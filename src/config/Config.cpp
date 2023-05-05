/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/27 13:55:04 by seokchoi          #+#    #+#             */
/*   Updated: 2023/05/04 16:54:04 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <stack>
#include "Config.hpp"
#include "CheckConfigValid.hpp"

Config::Config()
{
	this->_directives = std::vector<Directive>();
}

Config::~Config()
{
	for (size_t i = 0; i < this->_directives.size(); i++)
		this->_directives[i].block.clear();
	this->_directives.clear();
}

Directive Config::parseDirective(const std::string &line)
{
	Directive directive; // 디렉티브 객체
	std::string trimdLine;
	if (line.empty() || line[0] == '#') // 주석이거나 빈 줄인 경우
	{
		directive.name = "fail";
		return directive;
	}
	trimdLine = Config::trim(line);
	size_t pos = trimdLine.find(' '); // 첫 번째 공백의 위치를 찾는다.
	if (pos == std::string::npos)	  // 공백이 없는 경우
	{
		directive.name = "fail"; // 디렉티브의 이름을 저장
		return directive;
	}

	directive.name = trimdLine.substr(0, pos);								// 디렉티브의 이름을 저장
	size_t value_pos = trimdLine.find_first_not_of(" ", pos + 1);			// 공백이 아닌 문자를 찾는다.
	directive.value = trimdLine.substr(value_pos, line.size() - value_pos); // 디렉티브의 값을 저장

	if (directive.value.find("{") != std::string::npos)
		directive.value.erase(directive.value.find("{"), 1);
	return directive; // 블록이 있는 경우 블록이 존재하는 블록이 반환된다.
}

void Config::setBlock(std::ifstream &infile, std::vector<Directive> &directives)
{
	std::string line; // 한 줄씩 읽어올 문자열
	size_t pos;		  // 문자열에서 위치를 나타내는 변수
	std::stack<int> blockCheck;

	while (std::getline(infile, line)) // 한 줄씩 읽어오기
	{
		pos = line.find('#', 1);
		if (pos != std::string::npos)
			line = line.substr(0, pos);
		if (line.find("}") != std::string::npos)
			return;
		Directive directive = this->parseDirective(line);
		if (directive.name == "fail")
			continue;
		directives.push_back(directive);
		if (line.find("{") != std::string::npos)
		{
			setBlock(infile, directives.back().block);
		}
	}
}

void Config::parsedConfig(int argc, char const **argv)
{
	std::string filename;
	std::ifstream infile; // 파일 스트림

	if (argc != 1 && argc != 2)
	{
		std::cout << "Usage: ./webserv [config_file]" << std::endl;
		exit(1);
	}
	if (argc == 2)
	{
		if (!CheckConfigValid::Parse(argv[1]))
		{
			std::cout << "Error: Invalid config file" << std::endl;
			exit(1);
		}
		filename = argv[1];
	}
	else
	{
		if (!CheckConfigValid::Parse("src/config/default.conf"))
		{
			std::cout << "Error: Invalid config file" << std::endl;
			exit(1);
		}
		filename = "src/config/default.conf";
	}

	infile.open(filename);
	setBlock(infile, _directives);
	infile.close();
}

void Config::printDirectives(std::vector<Directive> directive, size_t tab)
{
	for (size_t i = 0; i < directive.size(); i++)
	{
		for (size_t j = 0; j < tab; j++)
		{
			std::cout << "\t";
		}
		std::cout << "  " << directive[i].name << " : " << directive[i].value << std::endl;
		if (directive[i].block.empty())
			continue;
		Config::printDirectives(directive[i].block, tab + 1);
	}
}

const std::vector<Directive> Config::getDirectives() const
{
	return this->_directives;
}

/*
 *	원하는 directive name을 찾아주는 함수
 *
 *	param : 담아줄 Directive vector
 *	param : 찾을 Directive vector
 *	param :	찾을 Directive의 name
 */
void Config::getAllDirectives(std::vector<Directive> &newDirectives, std::vector<Directive> directives, std::string dirName)
{
	for (size_t i = 0; i < directives.size(); i++)
	{
		if (directives[i].name == dirName)
		{
			newDirectives.push_back(directives[i]);
		}
		if (directives[i].block.empty())
			continue;
		Config::getAllDirectives(newDirectives, directives[i].block, dirName);
	}
}

std::string Config::trim(const std::string &str)
{
	std::size_t first = str.find_first_not_of(' ');
	if (first == std::string::npos)
	{
		return "";
	}
	std::size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}
