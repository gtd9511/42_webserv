/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 19:09:51 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/28 21:27:12 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

ServerInfo &ServerInfo::operator=(const ServerInfo &ref)
{
	if (this != &ref)
	{
		this->ports = ref.ports;
		this->sockets = ref.sockets;
		this->clientMaxBodySize = ref.clientMaxBodySize;
		this->serverName = ref.serverName;
		this->root = ref.root;
		this->index = ref.index;
		this->limitExcepts = ref.limitExcepts;
		this->locations = ref.locations;
		this->errorPage = ref.errorPage;
	}
	return (*this);
}