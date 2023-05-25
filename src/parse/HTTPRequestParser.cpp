#include "HTTPRequestParser.hpp"

HTTPRequestParser::HTTPRequestParser() : state_(METHOD) {}

/**
 * HTTP 요청 메세지를 파싱해서 구조체에 담아 반환
 *
 * @param data HTTP 요청 메세지가 담긴 문자열
 * @return 파싱된 HTTP 요청 구조체
 */
HTTPRequest *HTTPRequestParser::parse(const std::string &data)
{
    buffer_.clear();
    // std::cout << "buffer_ : [" << buffer_ << "]" << std::endl;
    buffer_ += data;
    std::cout << data << std::endl;
    state_ = METHOD;

    while (!buffer_.empty())
    {
        switch (state_)
        {
        case METHOD:
            if (!parseMethod())
                return NULL;
            break;
        case PATH:
            if (!parsePath())
                return NULL;
            break;
        case HTTP_VERSION:
            if (!parseHTTPVersion())
                return NULL;
            break;
        case HEADER_NAME:
            if (!parseHeaderName())
            {
                std::cout << "here1" << std::endl;
                // buffer_.erase(0, buffer_.length());
                // std::cout << buffer_.empty() << std::endl;
                return NULL;
                // state_ = COMPLETE;
            }
            break;
        case HEADER_VALUE:
            if (!parseHeaderValue())
            {
                // std::cout << "here2" << std::endl;
                // state_ = COMPLETE;
                // buffer_.erase(0, buffer_.length());
            }
            break;
        case BODY:
            if (!parseBody())
                return NULL;
            break;
        default:
            return NULL;
        }
    }

    if (state_ == COMPLETE)
    {
        HTTPRequest *request = new HTTPRequest;
        request->method = method_;
        request->path = path_;
        request->http_version = http_version_;
        // header가 존재하지 않는 경우 다시 요청 다시 받기 위함
        if (headers_.size() == 0)
            return request;
        request->headers = headers_;
        request->body = body_;
        request->addr = addr_;
        std::map<std::string, std::string>::iterator it = request->headers.find("Host");
        if (it != headers_.end())
        {
            size_t pos = it->second.find(":");
            request->port = strtod(it->second.substr(pos + 1, it->second.length()).c_str(), NULL);
            request->strPort = it->second.substr(pos + 1, it->second.length());
        }
        else
            request->port = -1;
        // printResult(*request);
        reset();
        return request;
    }

    return NULL;
}

/**
 * HTTP 요청 메세지에서 METHOD 관련 파싱
 *
 * @return 올바른 METHOD라면 구조체에 저장 후 true 반환, 올바르지 않은 METHOD일 경우 false 반환
 */
bool HTTPRequestParser::parseMethod()
{
    size_t pos = buffer_.find(' ');
    if (pos == std::string::npos)
        return false;
    std::string method_str = buffer_.substr(0, pos);
    if (method_str == "GET" || method_str == "HEAD" || method_str == "POST" || method_str == "PUT" || method_str == "PATCH" || method_str == "DELETE" || method_str == "CONNECT" || method_str == "TRACE" || method_str == "OPTIONS")
        method_ = method_str;
    else
        return false;
    state_ = PATH;
    buffer_.erase(0, pos + 1);
    return true;
}

/**
 * HTTP 요청 메세지에서 Path 관련 파싱
 *
 * @return 경로가 존재한다면 true 반환, 경로가 존재하지 않다면 false 반환
 */
bool HTTPRequestParser::parsePath()
{
    size_t pos = buffer_.find(' ');
    if (pos == std::string::npos)
        return false;
    path_ = buffer_.substr(0, pos);
    state_ = HTTP_VERSION;
    buffer_.erase(0, pos + 1);
    pos = path_.find("?");
    if (pos != std::string::npos)
        query_ = path_.substr(pos);
    return true;
}

size_t minPos(size_t p1, size_t p2, size_t p3)
{
    return (p1 < p2 && p1 < p3 ? p1 : (p2 < p3 && p2 < p1 ? p2 : p3));
}

/**
 * HTTP 요청 메세지에서 HTTP 버전 파싱
 *
 * @return HTTP 버전이 존재한다면 구조체에 저장 후 true 반환, 존재하지 않는다면 false 반환
 */
bool HTTPRequestParser::parseHTTPVersion()
{
    size_t pos1 = buffer_.find("\r");
    size_t pos2 = buffer_.find("\n");
    size_t pos3 = buffer_.find("\r\n");
    if (pos1 == std::string::npos && pos2 == std::string::npos && pos3 == std::string::npos)
        return false;
    size_t pos = minPos(pos1, pos2, pos3);
    http_version_ = buffer_.substr(0, pos);
    state_ = HEADER_NAME;
    // 지금까지 사용한 버퍼 지우기
    buffer_.erase(0, pos);
    // 버퍼 개행이 \n, \r, \r\n 에 따라 각각 처리
    if (buffer_.find("\n") == 0)
        buffer_.erase(0, 1);
    else if (buffer_.find("\r") == 0 && buffer_.find("\n") == 1)
        buffer_.erase(0, 2);
    else if (buffer_.find("\r") == 0)
        buffer_.erase(0, 1);
    if (buffer_.empty())
        state_ = COMPLETE;
    return true;
}

/**
 * HTTP 요청 메세지에서 HEADER의 이름 부분 파싱
 *
 * @return 해더의 이름이 존재한다면 현재 이름 저장 후 true 반환, 존재하지 않는다면 false 반환
 */
bool HTTPRequestParser::parseHeaderName()
{
    size_t pos = buffer_.find(':');
    // 만약 HTTP요청 메세지에서 헤더가 끝까지 제대로 오지 않는 경우, 그 이전 정보까지만 활용
    if (pos == std::string::npos)
    {
        state_ = COMPLETE;
        buffer_.clear();
        return true;
    }
    current_header_name_ = buffer_.substr(0, pos);
    buffer_.erase(0, pos + 1);
    state_ = HEADER_VALUE;
    return true;
}

/**
 * HTTP 요청 메세지에서 HEADER의 값 파싱.
 * 만약 BODY를 받을 필요가 없는 요청이라면 COMPLETE로 상태를 변경, BODY를 받아야하는 요청이면 BODY로 상태를 변경
 *
 * @return 헤더의 값이 존재한다면 이미 저장되어있는 헤더 이름과 함께 map에 저장 후 true 반환, 존재하지 않는다면 false 반환
 */
bool HTTPRequestParser::parseHeaderValue()
{
    size_t pos1 = buffer_.find("\r");
    size_t pos2 = buffer_.find("\n");
    size_t pos3 = buffer_.find("\r\n");
    if (pos1 == std::string::npos && pos2 == std::string::npos && pos3 == std::string::npos)
        return false;
    size_t pos = minPos(pos1, pos2, pos3);
    std::string header_value = buffer_.substr(1, pos);
    std::cout << "current_header_name_ : " << current_header_name_ << ", header_value : " << header_value << std::endl;
    headers_.insert(std::make_pair(current_header_name_, header_value));
    buffer_.erase(0, pos + 2);
    if (current_header_name_ == "Host")
    {
        pos = header_value.find(":");
        if (pos != std::string::npos)
            addr_ = header_value.substr(0, pos);
    }
    if (buffer_.substr(0, 2) == "\r\n")
    {
        buffer_.erase(0, 2);
        state_ = (method_ == "GET" || method_ == "HEAD" || method_ == "DELETE" || method_ == "CONNECT" || method_ == "TRACE" || method_ == "OPTIONS") ? COMPLETE : BODY;
    }
    else
    {
        state_ = HEADER_NAME;
    }
    return true;
}

/**
 * HTTP 요청 메세지에서 HTTP 버전 파싱
 *
 * @return HTTP 버전이 존재한다면 구조체에 저장 후 true 반환, 존재하지 않는다면 false 반환
 */
bool HTTPRequestParser::parseBody()
{
    if (method_ == "POST")
    {
        std::map<std::string, std::string>::iterator it =
            headers_.find("content-length");
        if (it != headers_.end())
        {
            int content_length = atoi(it->second.c_str());
            if (buffer_.size() < static_cast<size_t>(content_length))
                return false;
            body_ = buffer_.substr(0, content_length);
            buffer_.erase(0, content_length);
        }
    }
    state_ = COMPLETE;
    return true;
}

/**
 * HTTP 요청 메세지 파싱이 끝나면 사용한 버퍼를 모두 지워줌
 */
void HTTPRequestParser::reset()
{
    state_ = METHOD;
    headers_.clear();
    body_.clear();
    buffer_.clear();
    current_header_name_.clear();
}

/**
 * HTTP 요청 메세지에서 Content-Type 헤더의 값을 반환
 *
 * @param request 파싱된 HTTP 요청
 * @return 문자열의 Content-Type 값 혹은 빈 문자열
 */
std::string HTTPRequestParser::getContentType(const HTTPRequest &request)
{
    std::map<std::string, std::string> headers = request.headers;

    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
    {
        if (it->first == "Content-Type")
            return it->second;
    }
    return "";
}

/**
 * HTTP 요청 구조체 출력
 *
 * @param request 파싱된 HTTP 요청
 * @return void
 */
void HTTPRequestParser::printResult(const HTTPRequest &result)
{
    std::cout << "Request method: " << result.method << std::endl;
    std::cout << "Request path: " << result.path << std::endl;
    std::cout << "Request port: [" << result.port << "]" << std::endl;
    std::cout << "Request HTTP version: " << result.http_version << std::endl;

    for (std::map<std::string, std::string>::const_iterator it = result.headers.begin(); it != result.headers.end(); ++it)
        std::cout << "Header: " << it->first << " = " << it->second << std::endl;

    std::cout << "Body: " << result.body << std::endl;
}

int HTTPRequestParser::getPort(const HTTPRequest &result)
{
    return result.port;
}
