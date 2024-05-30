/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:13:08 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/30 18:46:37 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ModeHandler.hpp"

ModeHandler::ModeHandler(Server* server) : _server(server) {}

void ModeHandler::handleModeCommand(Client* client, const std::string& command)
{
	_server->log("Received MODE command: " + command, "\033[1;34m");
	std::vector<std::string> tokens = split(command, " \n\r\t");

	if (tokens.size() < 2) {
		_server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "MODE"));
		_server->log("MODE command error: Need more parameters", "\033[1;31m");
		return;
	}

	std::string target = tokens[1];
	if (target[0] == '#' || target[0] == '&') {
		_server->log("Handling channel mode for target: " + target, "\033[1;34m");
		handleChannelMode(client, tokens);
	} else {
		_server->log("Handling user mode for target: " + target, "\033[1;34m");
		handleUserMode(client, tokens);
	}
}

void ModeHandler::handleUserMode(Client* client, const std::vector<std::string>& tokens)
{
	_server->log("Entered handleUserMode", "\033[1;34m");
	std::string target = tokens[1];
	_server->log("User mode target: " + target, "\033[1;34m");
	if (target != client->getNickname()) {
		_server->sendToClient(client->getFd(), ERR_USERSDONTMATCH(client));
		_server->log("User mode error: Users don't match", "\033[1;31m");
		return;
	}

	if (tokens.size() == 2) {
		_server->sendToClient(client->getFd(), RPL_UMODEIS(client, ""));
		_server->log("Sent user modes to client", "\033[1;32m");
	} else {
		std::string modeString = tokens[2];
		_server->log("Updating user modes: " + modeString, "\033[1;34m");
		// Additional logging or mode handling logic can be added here
	}
}

void ModeHandler::handleChannelMode(Client* client, const std::vector<std::string>& tokens)
{
	_server->log("Entered handleChannelMode", "\033[1;34m");
	std::string channelName = tokens[1];
	_server->log("Channel mode target: " + channelName, "\033[1;34m");
	Channel* channel = _server->getChannelByName(channelName);
	if (!channel) {
		_server->sendToClient(client->getFd(), ERR_NOSUCHCHANNEL(client, channelName));
		_server->log("Channel mode error: No such channel " + channelName, "\033[1;31m");
		return;
	}

	if (tokens.size() == 2) {
		std::ostringstream oss;
		oss << RPL_CHANNELMODEIS(client, channel->getName(), channel->getModes());
		oss << RPL_CREATIONTIME(client, channel->getName(), channel->getTopicTime());
		_server->sendToClient(client->getFd(), oss.str());
		_server->log("Sent channel modes to client", "\033[1;32m");
		return;
	}

	if (!channel->isOperator(client)) {
		_server->sendToClient(client->getFd(), ERR_CHANOPRIVSNEEDED(client, channelName));
		_server->log("Channel mode error: Channel operator privileges needed for " + channelName, "\033[1;31m");
		return;
	}

	std::string modeString = tokens[2];
	bool adding = true;
	size_t argIndex = 3;

	for (size_t i = 0; i < modeString.length(); ++i) {
		char mode = modeString[i];
		if (mode == '+') {
			adding = true;
			_server->log("Adding mode: " + std::string(1, mode), "\033[1;34m");
		} else if (mode == '-') {
			adding = false;
			_server->log("Removing mode: " + std::string(1, mode), "\033[1;34m");
		} else if (mode == 'b' || mode == 'l' || mode == 'i' || mode == 'k' || mode == 't') {
			std::string argument;
			if (argIndex < tokens.size()) {
				argument = tokens[argIndex++];
			}
			setChannelMode(client, channel, std::string(1, mode), adding, argument);
		} else {
			_server->sendToClient(client->getFd(), ERR_UNKNOWNMODE(client, mode, channelName));
			_server->log("Unknown mode: " + std::string(1, mode), "\033[1;31m");
		}
	}
}

void ModeHandler::setChannelMode(Client *client, Channel* channel, const std::string& mode, bool adding, const std::string& argument)
{
	_server->log("Setting channel mode: " + mode + " Adding: " + (adding ? "true" : "false") + " Argument: " + argument, "\033[1;32m");
	if (mode == "l")
		{
			applyModeL(channel, adding, argument);
		}
	else if (mode == "i")
		{
			applyModeI(channel, adding);
		}
	else if (mode == "k")
		{
			applyModeK(channel, adding, argument);
		}
	else if (mode == "t")
		{
			applyModeT(channel, adding);
		}
	else if (mode == "o")
			applyModeO(client, channel, adding, argument);
}

void ModeHandler::applyModeL(Channel* channel, bool adding, const std::string& argument)
{
	if (adding) {
		int limit = std::atoi(argument.c_str());
		std::ostringstream oss;
		oss << limit;
		_server->log("Applying mode L: Setting limit to " + oss.str(), "\033[1;32m");
		channel->setClientLimit(limit);
	} else {
		_server->log("Applying mode L: Removing limit", "\033[1;31m");
		channel->setClientLimit(0);
	}
}

void ModeHandler::applyModeI(Channel* channel, bool adding)
{
	_server->log("Applying mode I: " + std::string(adding ? "Setting invite-only" : "Removing invite-only"), "\033[1;32m");
	channel->setInviteOnly(adding);
}

void ModeHandler::applyModeK(Channel* channel, bool adding, const std::string& argument)
{
	if (adding) {
		_server->log("Applying mode K: Setting key to " + argument, "\033[1;32m");
		channel->setKey(argument);
	} else {
		_server->log("Applying mode K: Removing key", "\033[1;31m");
		channel->setKey("");
	}
}

void ModeHandler::applyModeT(Channel* channel, bool adding)
{
	_server->log("Applying mode T: " + std::string(adding ? "Setting topic protection" : "Removing topic protection"), "\033[1;32m");
	channel->setTopicProtection(adding);
}

void ModeHandler::applyModeO(Client* client, Channel* channel, bool adding, const std::string& argument)
{
	Client* targetClient = _server->getClientByName(argument);
	if (!targetClient) {
		_server->sendToClient(client->getFd(), ERR_NOSUCHNICK(client, argument));
		_server->log("Mode o error: No such client " + argument, "\033[1;31m");
		return;
	}

	if (!channel->hasClient(targetClient)) {
		_server->sendToClient(client->getFd(), ERR_USERNOTINCHANNEL(client, argument, channel->getName()));
		_server->log("Mode o error: Client " + argument + " not in channel " + channel->getName(), "\033[1;31m");
		return;
	}

	if (adding) {
		_server->log("Applying mode o: Adding operator " + argument, "\033[1;32m");
		channel->addOperator(targetClient);
		std::string message = ":" + client->getNickname() + " MODE " + channel->getName() + " +o " + targetClient->getNickname() + "\r\n";
		channel->broadcast(message, NULL, _server);
	} else {
		_server->log("Applying mode o: Removing operator " + argument, "\033[1;31m");
		channel->removeOperator(targetClient);
		std::string message = ":" + client->getNickname() + " MODE " + channel->getName() + " -o " + targetClient->getNickname() + "\r\n";
		channel->broadcast(message, NULL, _server);
	}
}