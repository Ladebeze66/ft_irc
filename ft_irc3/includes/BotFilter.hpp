/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BotFilter.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 11:44:44 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/06/06 13:41:41 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOTFILTER_HPP
#define BOTFILTER_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class BotFilter
{
public:
	BotFilter(Server *server);
	void loadBadWords(const std::string &fileName);
	bool checkMessage(Client *client, Channel *channel, const std::string &message);

private:
	Server *_server;
	std::vector<std::string> _badWords;
	std::map<int, int> _warnings;

	bool containsBadWords(const std::string &message);
	void warnClient(Client *client, Channel *channel);
	void kickClient(Client *client, Channel *channel);
};

#endif
