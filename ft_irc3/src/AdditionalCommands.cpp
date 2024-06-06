/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AdditionalCommands.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:27:29 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/06/06 18:41:08 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AdditionalCommands.hpp"

AdditionalCommands::AdditionalCommands(Server *server) : _server(server) {}

void AdditionalCommands::processCommand(Client *client, const std::string &command)
{
	if (command.find("PART") == 0)
	{
		handlePartCommand(_server, client, command);
	}
	else if (command.find("PRIVMSG") == 0)
	{
		handlePrivmsgCommand(_server, client, command);
	}
	else if (command.find("WHO") == 0)
	{
		WhoHandler whoHandler(_server);
		whoHandler.handleWhoCommand(client, command);
	}
	else if (command.find("WHOIS") == 0)
	{
		WhoHandler whoHandler(_server);
		whoHandler.handleWhoisCommand(client, command);
	}
	else if (command.find("INVITE") == 0)
	{
		InviteHandler inviteHandler(_server);
		inviteHandler.handleInviteCommand(client, command);
	}
	else if (command.find("TOPIC") == 0)
	{
		TopicHandler topichandler(_server);
		topichandler.handleTopicCommand(client, command);
	}
	else if (command.find("LIST") == 0)
	{
		broadcastChannelList(client, _server);
	}
	else if (command.find("KICK") == 0)
	{
		KickHandler kickHandler(_server);
		kickHandler.handleKickCommand(client, command);
	}
	else
	{
		_server->sendToClient(client->getFd(), ERR_UNKNOWNCOMMAND(client, command));
		_server->log("Message from client " + client->getNickname() + ": " + command, MAGENTA);
	}
}

void AdditionalCommands::broadcastChannelList(Client *client, Server *server)
{
	std::map<std::string, Channel *> &channels = server->getChannels();
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		server->sendToClient(client->getFd(), RPL_LIST(client, it->first, it->second->getClients().size(), it->second->getTopic()));
	}
	server->sendToClient(client->getFd(), RPL_LISTEND(client));
}

// Fonction pour gérer la commande PART
void AdditionalCommands::handlePartCommand(Server *server, Client *client, const std::string &command)
{
	std::istringstream iss(command);
	std::string cmd, channelNames;
	iss >> cmd >> channelNames;

	if (channelNames.empty())
	{
		server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "PART"));
		return;
	}

	std::vector<std::string> channels = split(channelNames, ",");
	std::map<std::string, Channel *> &channelMap = server->getChannels();

	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string &channelName = channels[i];
		if (channelMap.find(channelName) == channelMap.end())
		{
			server->sendToClient(client->getFd(), ERR_NOSUCHCHANNEL(client, channelName));
			continue;
		}

		Channel *channel = channelMap[channelName];
		if (!channel->hasClient(client)) {
			server->sendToClient(client->getFd(), ERR_NOTONCHANNEL(client, channelName));
			continue;
		}

		channel->removeClient(client);

		std::ostringstream partMsg;
		partMsg << ":" << client->getNickname() << " PART " << channelName << "\r\n";
		server->sendToClient(client->getFd(), partMsg.str());

		if (channel->isEmpty())
		{
			delete channel;
			channelMap.erase(channelName);
		}

		server->log("Client " + client->getNickname() + " left channel " + channelName, MAGENTA);
	}
}

void AdditionalCommands::handlePrivmsgCommand(Server *server, Client *client, const std::string &command)
{
	std::istringstream iss(command);
	std::string cmd, target, message;
	iss >> cmd >> target;
	getline(iss, message);

	// Remove the initial ':' in the message if present
	if (!message.empty() && message[0] == ':')
		message = message.substr(1);

	if (target.empty())
	{
		server->sendToClient(client->getFd(), ERR_NORECIPIENT(client, "PRIVMSG"));
		return;
	}

	if (message.empty())
	{
		server->sendToClient(client->getFd(), ERR_NOTEXTTOSEND(client));
		return;
	}

	std::map<std::string, Channel *> &channels = server->getChannels();

	// Check if the target is a channel
	if (channels.find(target) != channels.end())
	{
		Channel *channel = channels[target];

		if (channel->isBanned(client))
		{
			server->sendToClient(client->getFd(), ERR_CANNOTSENDTOCHAN(client, target));
			return;
		}

		// Check the message with the bot filter before sending it
		if (!server->getBotFilter()->checkMessage(client, channel, message))
		{
			// If the message is inappropriate, the bot filter will handle warnings or kicks
			return;
		}

		std::vector<Client *> channelClients = channel->getClients();

		for (size_t i = 0; i < channelClients.size(); ++i)
		{
			if (channelClients[i] != client)
			{
				std::stringstream privMsg;
				privMsg << ":" << client->getNickname() << " PRIVMSG " << target << " :" << message << "\r\n";
				server->sendToClient(channelClients[i]->getFd(), privMsg.str());
			}
		}
	}
	else
	{
		Client *targetClient = server->getClientByName(target);

		if (targetClient)
		{
			// Check the message with the bot filter before sending it
			if (!server->getBotFilter()->checkMessage(client, NULL, message))
			{
				// If the message is inappropriate, the bot filter will handle warnings or kicks
				return;
			}

			std::stringstream privMsg;
			privMsg << ":" << client->getNickname() << " PRIVMSG " << target << " :" << message << "\r\n";
			server->sendToClient(targetClient->getFd(), privMsg.str());

			if (targetClient->isAway())
			{
				server->sendToClient(client->getFd(), RPL_AWAY(client, targetClient->getNickname(), targetClient->getAwayMessage()));
			}
		}
		else
		{
			// If the target is neither a channel nor an existing user, send an error message
			server->sendToClient(client->getFd(), ERR_NOSUCHNICK(client, target));
		}
	}
}
