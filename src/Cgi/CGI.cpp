/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sanghan <sanghan@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 17:29:58 by yje               #+#    #+#             */
/*   Updated: 2023/05/03 13:55:40 by sanghan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequestParser.hpp"
#include "CGI.hpp"

// CGI 환경변수 세팅

CGI::CGI()
{

}

void CGI::initEnvp(const HTTPRequest& request) // request config 이름 확인해서 받아오기
{
	// std::map<std::string, std::string> HTTPRequest.headers;//HTTPRequestParser확인하기

	// const std::string &method =
	// std::size_t content_length = request.getContentLength(); // contentlength
	// if (method == "POST" && content_length > 0)
	// {
	// 	headers["CONTENT_LENGTH"] = toString(content_length);
	// }
	this->envp_["AUTH_TYPE"] = "";
	// this->envp_["CONTENT_LENGTH"] = to_string(this->_body.length());
	// this->envp_["CONTENT_TYPE"] = request.getHeader("CONTENT-TYPE"); // 헤더가져오기
	this->envp_["GATEWAY_INTERFACE"] = "CGI/1.1";
	// this->envp_["HTTP_ACCEPT"] = ""
	this->envp_["PATH_INFO"] = request.path;
	this->envp_["PATH_TRANSLATED"] = request.path;
	// PATH_INFO의 변환. 스크립트의 가상경로를, 실제 호출 할 때 사용되는 경로로 맵핑.
	//  요청 URI의 PATH_INFO 구성요소를 가져와, 적합한 가상 : 실제 변환을 수행하여 맵핑.
	// this->envp_["QUERY_STRING"] = request.getQueryString(); // 쿼리스트링
	// this->envp_["REMOTE_ADDR"] = ""ip주소 받아와야 함
	this->envp_["REMOTE_IDENT"] = ""; //-> 권한 부여
	this->envp_["REMOTE_USER"] = "";
	// this->envp_["REQUEST_METHOD"] = method;
	this->envp_["REQUEST_URI"] = ""; // request.getUri();
	// this->envp_["SCRIPT_NAME"] = request.getUri();
	this->envp_["SERVER_NAME"] = ""; // 요청을 수신한 서버의 호스트 이름.
	this->envp_["SERVER_PORT"] = ""; // 요청을 수신한 서버의 포트 번호.
	this->envp_["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->envp_["SERVER_SOFTWARE"] = "webserv/1.1";
};

~AUTH_TYPE
	공백
-CONTENT_LENGTH
	content_Length
-CONTENT_TYPE
	content type
-GATEWAY_INTERFACE
	CGI/1.1()
~PATH_INFO
	request.get Uri()
~PATH_TRANSLATED
	request.get Uri()
-QUERY_STRING
	getQueryFullPath
-REMOTE_ADDR
	getClientIP
~REMOTE_IDENT
	공백
~REMOTE_USER
	공백
-REQUEST_METHOD
	req.method
~REQUEST_URI
	getUri()
-SCRIPT_NAME
	webserv/1.1()
-SERVER_NAME
	server.\_serverName
-SERVER_PORT
	server.\_serverPort
-SERVER_PROTOCOL
	HTTP/1.1()
-SERVER_SOFTWARE
	webserv/1.1()
