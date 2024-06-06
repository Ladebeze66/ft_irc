/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BotFilter.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 11:45:43 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/06/06 21:08:25 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BotFilter.hpp"

BotFilter::BotFilter(Server *server) : _server(server) {}

void BotFilter::loadBadWords(const std::string &fileName)
{
	std::ifstream file(fileName.c_str());
	std::string line;
	while (std::getline(file, line))
	{
		_badWords.push_back(line);
	}
}

bool BotFilter::checkMessage(Client *client, Channel *channel, const std::string &message)
{
	if (containsBadWords(message))
	{
		warnClient(client, channel);
		return false;  // Message is not allowed
	}
	return true;  // Message is allowed
}

bool BotFilter::containsBadWords(const std::string &message)
{
	for (std::vector<std::string>::iterator it = _badWords.begin(); it != _badWords.end(); ++it)
	{
		if (message.find(*it) != std::string::npos)
		{
			return true;
		}
	}
	return false;
}

void BotFilter::warnClient(Client* client, Channel *channel)
{
	int fd = client->getFd();
	_warnings[fd] += 1;
	if (_warnings[fd] >= 3)
	{
		kickClient(client, channel);
	}
	else
	{
		std::ostringstream oss;
		oss << "Warning: Inappropriate language detected. You have " << (3 - _warnings[fd]) << " warnings left before you are kicked.\r\n";
		_server->sendToClient(fd, BOTMESSAGE(client, channel->getName(), oss.str()));
	}
}

void BotFilter::kickClient(Client *client, Channel *channel)
{
	channel->banClient(client, channel); // Ensure the client is banned
	_server->sendToClient(client->getFd(), BOTMESSAGE(client, channel->getName(), "You have been kicked for inappropriate language.\r\n"));
	_server->disconnectClient(client->getFd());
}
