/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/23 20:31:14 by sunhwang          #+#    #+#             */
/*   Updated: 2023/04/24 20:25:29 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MASTER_HPP
#define MASTER_HPP

#include <sys/event.h>
#include <vector>
#include "Config.hpp"

class Master
{
private:
	Config config;
	std::vector<struct kevent> events;

public:
	const int kq;
	Master();
	~Master();
	std::vector<struct kevent> &getEvents();
};

#endif
