/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 17:35:20 by yje               #+#    #+#             */
/*   Updated: 2023/05/04 18:15:52 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <map>
#include <unistd.h>
#include <cctype>
#include <sys/stat.h>
class CGI
{
	// private:
	//
public:
	// CGI();				 //
	// CGI(const CGI &src); //
	// CGI &operator=(const CGI &src);
	// ~CGI(void); //
	// cgiRequest();
	// void initEnvp(const HTTPRequest &request);
	void initEnvp();

	void excuteCGI(const std::string &context);
	char **ENVPChangeStringArray();
	void setFilePath();
	void setBody(const std::string &body);
	std::string getBody() const;
	std::string getResponseBody() const;
	void setEnv(const std::map<std::string, std::string> &env);
	CGI(const std::string &cgi_path);
	bool isCgiPath(void) const;
	;

	std::map<std::string, std::string> envp_;
	std::string cgiPath_;
	std::string body_;
	int fileFD_[2];
};
#endif
//