/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 14:47:40 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/03 14:24:18 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_PARSER
#define HTTP_REQUEST_PARSER

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

struct HTTPRequest
{
    std::string method;
    std::string path;
    std::string http_version;
    std::map<std::string, std::string> headers;
    std::string body;
};

class HTTPRequestParser
{
private:
    enum ParseState
    {
        METHOD,
        PATH,
        HTTP_VERSION,
        HEADER_NAME,
        HEADER_VALUE,
        BODY,
        COMPLETE
    };

    ParseState state_;
    std::string method_;
    std::string path_;
    std::string http_version_;
    std::map<std::string, std::string> headers_;
    std::string body_;
    std::string buffer_;
    std::string current_header_name_;

    bool parseMethod();
    bool parsePath();
    bool parseHttpVersion();
    bool parseHeaderName();
    bool parseHeaderValue();
    bool parseBody();
    void reset();

public:
    HTTPRequestParser();

    HTTPRequest *parse(const std::string &data);
    std::string getContentType(const HTTPRequest &request);

    void printResult(const HTTPRequest &request);
};

#endif