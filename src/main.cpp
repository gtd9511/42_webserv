/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/03 13:47:00 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DefaultConfig.hpp"
#include "Master.hpp"
#include "Worker.hpp"
#include "Config.hpp"
#include "CheckConfigValid.hpp"
#include "ParsedConfig.hpp"

int main(int, char const *[])
{
    (void)argc;
    (void)argv;

    // Nginx Config file parsing
    ParsedConfig config;
    config.parsedConfig(argc, argv);
    // config.printConfig(config.getDirectives(), 0); // Directive 출력해보기

    Master master;
    Worker worker(master);
    worker.run();
    return 0;
}
