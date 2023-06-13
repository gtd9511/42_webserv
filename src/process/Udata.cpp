/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Udata.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 15:55:39 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/13 16:55:16 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Udata.hpp"

UData::UData(int fd, bool keepAlive)
{
	this->fd = fd;
	this->keepAlive = keepAlive;
	this->max = -1;
	this->timeout = -1;
	this->sessionID = "";
	this->alreadySessionSend = false;
	this->sesssionValid = false;
	this->expireTime = "";
	this->wantToDeleteSessionInCookie = false;
	this->request = "";
	this->result = NULL;
}