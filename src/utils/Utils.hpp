/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 15:15:56 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/06 15:30:50 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Utils_HPP
#define Utils_HPP

#include <iostream>
#include <string>
#include <stdio.h>

class Utils
{
public:
    /*
     * A default constructor
     */
    Utils();

    /*
     * A destructor
     */
    ~Utils();

    /*
     * Add it if you feel necessary additional member functions.
     */
    static size_t minPos(size_t p1, size_t p2, size_t p3);
};

#endif // Utils_HPP
