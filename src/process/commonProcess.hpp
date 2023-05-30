/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonProcess.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:20:30 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/30 21:18:38 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_PROCESS_HPP
#define COMMON_PROCESS_HPP

#include <string>
#include "Response.hpp"

#define GET "GET"
#define HEAD "HEAD"
#define POST "POST"
#define PUT "PUT"
#define PATCH "PATCH"
#define DELETE "DELETE"
#define CONNECT "CONNECT"
#define TRACE "TRACE"
#define OPTIONS "OPTIONS"

void ftSend(const int &socket, const std::string &buffer);
void ftSend(ResponseData *response, const std::string &response_header);
bool isDirectory(const std::string &path);
bool isFile(const std::string &path);
bool isMethod(const std::string &method);
bool writeFile(const std::string &path, const std::string &contents);
std::string readFile(const std::string &path);
const std::string getContentType(const HTTPRequest &request);
bool isEqual(const std::string &s1, const std::string &s2);

#endif