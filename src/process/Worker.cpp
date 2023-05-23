/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:10:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/23 14:52:14 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Worker.hpp"

Worker::Worker(Master &master) : kq(master.kq), signal(master.getEvents()), event_list(master.getEvents()), config(master.getConfig()), server(master.getServer())
{
	// Create sockets
	for (size_t i = 0; i < server.server.size(); i++)
	{
		for (size_t j = 0; j < server.server[i].port.size(); j++)
		{
			sockets.push_back(new Socket(master.getEvents(), server.server[i].port[j]));
		}
	}
}

Worker::~Worker()
{
	for (size_t i = 0; i < sockets.size(); i++)
		delete (sockets[i]);
}

void Worker::eventEVError(int k)
{
	// 서버 소켓 에러
	if (fd == sockets[k]->server_fd)
		error_exit("Server socket error");
	else
	{
		// 클라이언트 소켓 에러 아니면 다른 에러
		if (clients.find(fd) != clients.end())
			sockets[k]->disconnectClient(fd, clients);
	}
}

bool Worker::eventFilterRead(int k)
{
	found = std::find(sockets[k]->clientFds.begin(), sockets[k]->clientFds.end(), fd);
	if (found == sockets[k]->clientFds.end())
		return false;
	if (fd == sockets[k]->server_fd)
	{
		int client_fd = sockets[k]->handleEvent(event_list);
		clients[client_fd].clear();
	}
	else if (clients.find(fd) != clients.end())
	{
		char buf[1024];
		int n = 1;
		while (0 < (n = read(fd, buf, sizeof(buf))))
		{
			buf[n] = '\0';
			clients[fd] += buf;
		}
		if (n < 1)
		{
			// HTML 요청 메세지 보기
			// std::cout << "Received data from " << fd << ": " << clients[fd] << std::endl;
			struct kevent new_event;
			EV_SET(&new_event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			event_list.push_back(new_event);
		}
	}
	return true;
}

bool Worker::eventFilterWrite(int k)
{
	found = std::find(sockets[k]->clientFds.begin(), sockets[k]->clientFds.end(), fd);
	if (found == sockets[k]->clientFds.end())
		return false;
	HTTPRequest *result = parser.parse(clients[fd]);
	if (clients.find(fd) != clients.end())
	{
		if (result)
		{
			this->requestHandler(*result, fd);
		}
		else
			std::cout << "Failed to parse request" << std::endl;
		sockets[k]->disconnectClient(fd, clients);
		clients[fd].clear();
	}
	if (result)
		delete result;
	return true;
}

void Worker::run()
{
	struct kevent events[10];
	struct kevent event;
	int nevents;

	while (true)
	{
		// std::cout << "here" << std::endl;
		nevents = kevent(kq, &event_list[0], event_list.size(), events, 10, NULL);
		if (nevents == -1)
		{
			std::cerr << "Error waiting for events: " << strerror(errno) << std::endl;
			break;
		}
		event_list.clear();

		for (size_t k = 0; k < sockets.size(); k++)
		{
			for (int i = 0; i < nevents; i++)
			{
				event = events[i];
				fd = event.ident;

				if (event.flags & EV_ERROR)
					eventEVError(k);
				if (event.filter == EVFILT_READ)
				{
					if (eventFilterRead(k) == false)
						continue;
				}
				else if (event.filter == EVFILT_WRITE)
				{
					if (eventFilterWrite(k) == false)
						continue;
				}
				else if (event.filter == EVFILT_SIGNAL)
					signal.handleEvent(event, sockets);
			}
		}
	}
}

/*
 * 각각 method 실행과 해당 포트에 response를 보내줌
 *
 * @param request request 를 파싱완료한 구조체
 * @param client_fd 서버의 fd
 */
void Worker::requestHandler(const HTTPRequest &request, int client_fd)
{
	Response responseClass(request.port, this->server);
	ResponseData *response = responseClass.getResponseData(request, client_fd);
	if (std::find(response->limitExcept.begin(), response->limitExcept.end(), request.method) == response->limitExcept.end()) // limitExcept에 method가 없는 경우
	{
		// 현재는 location을 찾지못해 limit.except에서 판별이안되 넘어오는 경우도있음!
		// 잘못된 메서드일경우
		// method not allowed
		std::string response_body = "Method not allowed";
		std::string response_header = generateErrorHeader(405, response_body);
		write(response->clientFd, response_header.c_str(), response_header.length());
		write(response->clientFd, response_body.c_str(), response_body.length());
		delete response;
		return;
	}
	// 현재 메서드와 limit을 비교후 바로 404 갈지 실행한지 분기
	if (response->method == "GET")
	{
		if (isCGIRequest(response))
		{
			std::cout << "request.path : " << request.path << std::endl;
			std::cout << "response.path : " << response->path << std::endl;
			CGI cgi("");
			std::string cgiFullPath = "./src" + request.path + ".py";

			// test
			std::string filename = "result.html";
			std::string filepath = "./assets/html/";
			std::string fullpath = filepath + filename;
			// 파일을 열고 문자열을 쓴 후 닫습니다.
			std::ofstream testCGI(fullpath);

			std::cout << "cgipath -> full :  " << cgiFullPath << std::endl;
			testCGI << cgi.excuteCGI(cgiFullPath);
			testCGI.close();
		}
		getResponse(response);
	}
	else if (response->method == "POST")
	{
	}
	else // DELETE
	{
	}
	delete response;
}

bool Worker::isCGIRequest(ResponseData *response)
{
	// 이 부분은 CGI 요청을 확인하는 로직을 구현합니다.
	// 예를 들어, 요청 URL에 특정 확장자(.cgi, .php 등)가 포함되어 있는지 확인할 수 있습니다.
	// 요청이 CGI 요청인 경우 true를 반환하고, 그렇지 않은 경우 false를 반환합니다.
	// return request.find(".py") != std::string::npos;
	size_t pos = response->path.find("cgi-bin");
	return (pos != std::string::npos);
}

/**
 * GET request일 경우, response에 보내줄 리소스를 찾고 담긴 내용을 가져옴. 파일이 존재하지않으면 에러페이지 반환
 *
 * @param request 파싱된 HTTP 요청 메세지 구조체
 * @param client_fd 웹 소켓
 */
void Worker::getResponse(ResponseData *response)
{
	struct stat st;
	// if (!stat(response->resourcePath.c_str(), &st)) // 파일인지 디렉토리인지 검사하기위해 stat함수 사용
	// 	std::cerr << "Failed to get information about " << response->resourcePath.c_str() << std::endl;
	stat(response->resourcePath.c_str(), &st);
	if (!S_ISREG(st.st_mode)) // root + index을 검사해 파일이 아닐시 if로 분기
	{
		response->resourcePath = response->root + response->path; // root + path로 다시 검사
		std::memset(&st, 0, sizeof(st));
		// if (!stat(response->resourcePath.c_str(), &st))
		// 	std::cerr << "Failed to get information about " << response->resourcePath.c_str() << std::endl;
		stat(response->resourcePath.c_str(), &st);
		if (!S_ISREG(st.st_mode))
		{
			if (response->autoindex)
				return broad(response);
			else
				return errorResponse(response->clientFd);
		}
	}
	std::ifstream resource_file(response->resourcePath); // 위에서 stat함수로 파일검사는 완료
	if (!resource_file.is_open())						 // 혹시 open이 안될수있으니 한번더 체크
		return errorResponse(response->clientFd);
	// 경로에서 확장자 찾아준 뒤, Content-Type 찾기
	std::vector<std::string> tokens;
	std::istringstream iss(response->resourcePath);
	std::string token;
	while (std::getline(iss, token, '.'))
		tokens.push_back(token);
	std::string extension = tokens.back();
	MimeTypesParser mime(this->config);
	std::string contentType = mime.getMimeType(extension);
	std::string resource_content((std::istreambuf_iterator<char>(resource_file)),
								 std::istreambuf_iterator<char>());
	std::string response_header = generateHeader(resource_content, contentType);
	write(response->clientFd, response_header.c_str(), response_header.length());
	write(response->clientFd, resource_content.c_str(), resource_content.length());
	resource_file.close();
}

/**
 * 404 에러일 경우 나와야할 페이지 띄워주는 함수
 *
 * @param client_fd 브라우저 포트번호
 */
void Worker::errorResponse(int client_fd)
{
	std::string error_path = "./assets/html/404.html";
	std::ifstream error_file(error_path);
	std::string error_content((std::istreambuf_iterator<char>(error_file)),
							  std::istreambuf_iterator<char>());
	std::string error_header = generateErrorHeader(404, error_content);
	write(client_fd, error_header.c_str(), error_header.length());
	write(client_fd, error_content.c_str(), error_content.length());
}

/**
 * response의 헤더에 적어줄 내용을 만듬
 *
 * @param content getResource함수에서 찾아온 내용을 가져옴
 * @param contentType Content-Type
 * @return 최종완성된 헤더를 반환함
 */
std::string Worker::generateHeader(const std::string &content, const std::string &contentType)
{
	HTTPRequestParser parser;
	std::ostringstream oss;

	oss << "HTTP/1.1 200 OK\r\n";
	oss << "Content-Length: " << content.length() << "\r\n";
	oss << "Content-Type: " << contentType << "\r\n"; // MIME type can be changed as needed
	oss << "Connection: close\r\n\r\n";
	return oss.str();
}

/**
 * response의 헤더에 적어줄 내용을 만듬
 *
 * @param request request 를 파싱완료한 구조체
 * @param content getResource함수에서 찾아온 내용을 가져옴
 * @return 최종완성된 헤더를 반환함
 */
std::string Worker::generateErrorHeader(int status_code, const std::string &message)
{
	std::ostringstream oss;
	// oss << "HTTP/1.1 " << status_code << " " << message << "\r\n";
	oss << "HTTP/1.1 " << status_code << " OK\r\n";
	oss << "Content-Length: " << message.length() << "\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Connection: close\r\n\r\n";
	return oss.str();
}

void Worker::broad(ResponseData *response)
{
	std::stringstream broadHtml;
	broadHtml << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>broad page</title></head><body><h1>show</h1>";
	DIR *dirPtr = NULL;
	dirent *file;
	// if ((dirPtr = opendir(response->root.c_str())) != NULL)
	if ((dirPtr = opendir(response->root.c_str())) == NULL)
	{
		std::cout << "broad: location path err" << std::endl;
		return;
	}
	while ((file = readdir(dirPtr)))
	{
		broadHtml << "<p><a href=" << response->locationName << "/" << file->d_name << ">" << file->d_name << "</a></p>";
	}
	broadHtml << "</body></html>";
	std::string tmp = broadHtml.str();
	/* 헤더를 작성해주는과정 */
	MimeTypesParser mime(config);
	std::string contentType = mime.getMimeType("html");
	std::string response_header = generateHeader(tmp, contentType);
	write(response->clientFd, response_header.c_str(), response_header.length());
	write(response->clientFd, tmp.c_str(), tmp.length()); // 완성된 html 을 body로 보냄
}
