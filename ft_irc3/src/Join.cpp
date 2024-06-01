/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 19:51:31 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/21 20:13:30 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Join.hpp"

void JoinHandler::handleJoinCommand(Client* client, const std::string& params, Server* server)
{
	std::map<std::string, Channel*>& channels = server->getChannels();
	Channel* channel;

	server->log("Received JOIN command with params: " + params, RED);

	// Split params into channel names and keys
	std::vector<std::string> parts = split(params, " ");
	server->log("Split parts: ", RED);
	for (size_t i = 0; i < parts.size(); ++i)
	{
		std::ostringstream oss;
		oss << "Part " << i << ": " << parts[i];
		server->log(oss.str(), RED);
	}
	if (parts.empty())
	{
		server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "JOIN"));
		return;
	}

	std::string channelNames = parts[0];
	std::string keys = (parts.size() > 1) ? parts[1] : "";

	server->log("Split params: channelNames = " + channelNames + ", keys = " + keys, RED);

	std::vector<std::string> channelList = split(channelNames, ",");
	std::vector<std::string> keyList = split(keys, ",");

	server->log("channelList: ", RED);
	for (size_t i = 0; i < channelList.size(); ++i)
	{
		server->log(channelList[i], RED);
	}

	server->log("keyList: ", RED);
	for (size_t i = 0; i < keyList.size(); ++i)
	{
		server->log(keyList[i], RED);
	}

	for (size_t i = 0; i < channelList.size(); ++i)
	{
		const std::string& channelName = channelList[i];
		server->log("Processing channel: " + channelName, RED);

		if (channelName.empty())
		{
			server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "JOIN"));
			return;
		}

		if (channelName[0] == '0' && channelName.size() == 1)
		{
			// Leave all channels
			return;
		}

		if (channelName[0] != '#' && channelName[0] != '&')
		{
			server->sendToClient(client->getFd(), ERR_NOSUCHCHANNEL(client, channelName));
			return;
		}

		if (channels.find(channelName) == channels.end())
		{
			channel = new Channel(channelName);
			channels[channelName] = channel;
			channel->addOperator(client); // Set client as operator for new channel
		}
		else
		{
			channel = channels[channelName];
		}

		if (channel->isBanned(client))
		{
			server->sendToClient(client->getFd(), ERR_BANNEDFROMCHAN(client, channelName));
			return;
		}

		if (channel->isFull())
		{
			server->sendToClient(client->getFd(), ERR_CHANNELISFULL(client, channelName));
			return;
		}

		if (channel->isInviteOnly() && !channel->isInvited(client))
		{
			server->sendToClient(client->getFd(), ERR_INVITEONLYCHAN(client, channelName));
			return;
		}

		// Extract the key for this channel, if provided
		std::string key = (i < keyList.size()) ? keyList[i] : "";
		server->log("Using key: " + key + " for channel: " + channelName, RED);

		if (!channel->checkKey(key))
		{
			server->sendToClient(client->getFd(), ERR_BADCHANNELKEY(client, channelName));
			return;
		}

		channel->addClient(client);
		sendJoinSuccess(client, channel, server);
	}
}

void JoinHandler::sendJoinSuccess(Client* client, Channel* channel, Server* server)
{
	std::vector<Client*> clients = channel->getClients();
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		std::string joinMsg = ":" + client->getNickname() + " JOIN " + channel->getName() + "\r\n";
		server->sendToClient((*it)->getFd(), joinMsg);
	}

	if (!channel->getTopic().empty())
	{
		server->sendToClient(client->getFd(), RPL_TOPIC(client, channel->getName(), channel->getTopic()));
		server->sendToClient(client->getFd(), RPL_TOPICWHOTIME(client, channel->getName(), channel->getTopicSetter(), channel->getTopicTime()));
	}
	else
	{
		server->sendToClient(client->getFd(), RPL_NOTOPIC(client, channel->getName()));
	}

	std::ostringstream oss;
	oss << RPL_CHANNELMODEIS(client->getFd(), channel->getName(), channel->getModes());
	oss << RPL_CREATIONTIME(client, channel->getName(), channel->getTopicTime());
	server->sendToClient(client->getFd(), oss.str());

	std::string usersList = getUsersList(channel);
	server->sendToClient(client->getFd(), usersList);
}

std::string JoinHandler::getUsersList(Channel* channel)
{
	std::vector<Client*> clients = channel->getClients();
	std::string users;
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (channel->isOperator((*it)))
			users += "@";
		users += (*it)->getNickname() + " ";
	}

	std::ostringstream oss;
	oss << RPL_NAMREPLY(clients[0], channel->getName(), users);
	oss << RPL_ENDOFNAMES(clients[0], channel->getName());
	return oss.str();
}
