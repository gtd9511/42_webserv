/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConfigValid.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 21:02:55 by seokchoi          #+#    #+#             */
/*   Updated: 2023/04/30 18:42:16 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CheckConfigValid.hpp"

std::stack<std::string> CheckConfigValid::_block_stack;

CheckConfigValid::CheckConfigValid() {}

CheckConfigValid::~CheckConfigValid() {}

bool CheckConfigValid::Parse(const std::string &file_path)
{
	std::ifstream file_stream(file_path);
	if (!file_stream.is_open())
	{
		std::cerr << "Failed to open file: " << file_path << std::endl;
		return false;
	}
	try
	{
		CheckConfigValid::ParseStream(file_stream);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error parsing config file: " << e.what() << std::endl;
		return false;
	}
	return true;
}

void CheckConfigValid::ParseStream(std::istream &input_stream)
{
	std::string line;
	while (std::getline(input_stream, line))
	{
		line = Trim(line);
		if (line.empty())
		{
			continue;
		}
		if (line.front() == '#')
		{
			continue;
		}
		if (line.front() == '}')
		{
			if (_block_stack.empty())
			{
				throw std::runtime_error("Unexpected '}'");
			}
			_block_stack.pop();
			continue;
		}
		size_t found = line.find('{');
		if (found == std::string::npos)
		{
			if (_block_stack.empty())
			{
				throw std::runtime_error("Unexpected directive: " + line);
			}
		}
		else
		{
			if (found != line.size() - 1)
			{
				throw std::runtime_error("Unexpected character after '{'");
			}
			std::string block_name = line.substr(0, found);
			block_name = Trim(block_name);
			if (block_name.empty())
			{
				throw std::runtime_error("Empty block name");
			}
			_block_stack.push(block_name);
		}
	}
	if (!_block_stack.empty())
	{
		throw std::runtime_error("Missing '}'");
	}
}

std::string CheckConfigValid::Trim(const std::string &str) // 양쪽 끝의 공백 제거
{
	std::size_t first = str.find_first_not_of(' ');
	if (first == std::string::npos)
	{
		return "";
	}
	std::size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}