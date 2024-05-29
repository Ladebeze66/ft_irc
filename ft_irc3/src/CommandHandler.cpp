/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 18:26:34 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/28 15:30:11 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

CommandHandler::CommandHandler(Server *server)
	: _server(server), _additionalCommands(new AdditionalCommands(server))
	{
		// Ensure that _server is not null
		if (!_server)
		{
			std::cerr << "Server pointer is null in CommandHandler constructor." << std::endl;
			exit(EXIT_FAILURE);
		}
	}

void CommandHandler::handleCommand(Client* client, const std::string& command)
{
	std::vector<std::string> tokens = split(command, " \n\r\t");

	if (tokens.empty()) {
		return;
	}

	std::string commandType = tokens[0];
	if (commandType == "CAP") {
		handleCapCommand(client, tokens);
	} else if (commandType == "PASS") {
		handlePassCommand(client, tokens);
	} else if (commandType == "NICK") {
		handleNick(client, tokens);
	} else if (commandType == "USER") {
		handleUser(client, tokens);
	} else if (commandType == "QUIT") {
		handleQuitCommand(client, tokens);
	} else if (commandType == "PING")
	{
		handlePingCommand(client, tokens);
	}
	else if (commandType == "ERROR") {
		if (tokens.size() > 1) {
			handleErrorCommand(client, tokens[1]);
		}
	}
	else if (commandType == "JOIN") {
		JoinHandler joinHandler;
		joinHandler.handleJoinCommand(client, tokens[1], _server);
	}
	else {
		if (!client->isAuthenticated()) {
			_server->sendToClient(client->getFd(), ERR_NOTREGISTERED(client));
			_server->log("Client " + client->getNickname() + " attempted to send a command before registering.", RED);
		} else {
			_additionalCommands->processCommand(client, command);
		}
	}
	std::cout << "Client " << client->getFd() << " " << client->getNickname() << " " << client->getUser() << " " << client->getPassword() << " " << client->getRealName() << std::endl;
}

void CommandHandler::handleCapCommand(Client* client, const std::vector<std::string>& tokens)
{
	if (tokens.size() < 2)
	{
		_server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "CAP"));
		return;
	}

	std::string subcommand = tokens[1];
	std::string capabilities = "multi-prefix extended-join account-notify batch invite-notify";

	if (subcommand == "LS")
	{
		_server->sendToClient(client->getFd(), RPL_CAP(client->getFd(), "LS", capabilities));
	}
	else if (subcommand == "LIST")
	{
		_server->sendToClient(client->getFd(), RPL_CAP(client->getFd(), "LIST", capabilities));
	}
	else if (subcommand == "REQ")
	{
		if (tokens.size() < 3)
		{
			_server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "CAP"));
			return;
		}
		std::string requestedCapabilities = tokens[2];
		// For simplicity, we assume all requested capabilities are accepted
		_server->sendToClient(client->getFd(), RPL_CAP(client->getFd(), "ACK", requestedCapabilities));
	}
	else if (subcommand == "END")
	{
		_server->sendToClient(client->getFd(), RPL_CAPEND(client->getFd()));
	}
	else
	{
		_server->sendToClient(client->getFd(), ERR_UNKNOWNCOMMAND(client, "CAP"));
	}
}

void CommandHandler::handlePassCommand(Client* client, const std::vector<std::string>& tokens)
{
	if (tokens.size() < 2) {
		_server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "PASS"));
		return;
	}

	if (client->isAuthenticated()) {
		_server->sendToClient(client->getFd(), ERR_ALREADYREGISTERED(client));
		return;
	}

	if (tokens[1] == _server->_password) {
		client->setPassword(tokens[1]);
		_server->sendToClient(client->getFd(), ":server NOTICE * :Password accepted\r\n");
		_server->log("Client " + client->getNickname() + " provided correct password.", GREEN);
	} else {
		_server->sendToClient(client->getFd(), ERR_PASSWDMISMATCH(client));
		_server->log("Client " + client->getNickname() + " failed authentication password.", RED);
		// Optionally disconnect the client here
	}
}

bool CommandHandler::isValidNickname(const std::string& nickname)
{
	// Implement nickname validation according to IRC protocol
	if (nickname.empty() || nickname[0] == '#' || nickname[0] == ':' || nickname.find(' ') != std::string::npos) {
		return false;
	}
	return true;
}

bool CommandHandler::isNicknameInUse(const std::string& nickname)
{
	for (std::map<int, Client*>::iterator it = _server->_clients.begin(); it != _server->_clients.end(); ++it) {
		if (it->second->getNickname() == nickname) {
			return true;
		}
	}
	return false;
}

void CommandHandler::handleNick(Client* client, const std::vector<std::string>& tokens)
{
	if (tokens.size() < 2) {
		_server->sendToClient(client->getFd(), ERR_NONICKNAMEGIVEN(client));
		return;
	}

	std::string newNick = tokens[1];

	if (!isValidNickname(newNick)) {
		_server->sendToClient(client->getFd(), ERR_ERRONEUSNICKNAME(client, newNick));
		return;
	}

	if (isNicknameInUse(newNick)) {
		_server->sendToClient(client->getFd(), ERR_NICKNAMEINUSE(client, newNick));
		return;
	}

	std::string oldNick = client->getNickname();
	client->setNickname(newNick);

	// Envoyer le message NICK à tous les clients connectés
	std::string nickMessage = ":" + oldNick + " NICK " + newNick + "\r\n";
	for (std::map<int, Client*>::iterator it = _server->_clients.begin(); it != _server->_clients.end(); ++it) {
		_server->sendToClient(it->second->getFd(), nickMessage);
	}

	std::ostringstream oss;
	oss << "Client " << client->getFd() << " changed nickname from " << oldNick << " to " << newNick;
	_server->log(oss.str(), GREEN);
}


void CommandHandler::handleUser(Client* client, const std::vector<std::string>& tokens)
{
	if (tokens.size() < 5) {
		_server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "USER"));

		std::ostringstream oss;
		oss << "Client " << client->getFd() << ": USER command failed - not enough parameters.";
		_server->log(oss.str(), RED);

		return;
	}

	if (client->isAuthenticated()) {
		_server->sendToClient(client->getFd(), ERR_ALREADYREGISTERED(client));

		std::ostringstream oss;
		oss << "Client " << client->getFd() << ": USER command failed - already registered.";
		_server->log(oss.str(), RED);

		return;
	}

	std::string username = tokens[1];
	std::string realname = tokens[4].substr(1); // remove leading ':'

	client->setUser(username);
	client->setRealName(realname);

	std::ostringstream oss;
	oss << "Client " << client->getFd() << ": USER command set username to " << username << " and real name to " << realname;
	_server->log(oss.str(), BLUE);

	if (client->getPassword() == _server->_password && !client->getNickname().empty()) {
		client->authenticate();
		WelcomeHandler welcomeHandler;
		welcomeHandler.sendWelcomeMessages(client, _server);
		_server->log("Client " + client->getNickname() + " authenticated.", GREEN);
	} else {
		std::ostringstream oss;
		oss << "Client " << client->getFd() << ": USER command failed - authentication conditions not met.";
		_server->log(oss.str(), RED);
	}
}

void CommandHandler::handlePingCommand(Client* client, const std::vector<std::string>& tokens)
{
	if (tokens.size() < 2)
	{
		_server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "PING"));
		return;
	}

	std::string serverName = tokens[1];
	std::string pongMessage = ":" + serverName + " PONG " + serverName + "\r\n";
	_server->sendToClient(client->getFd(), pongMessage);
}


void CommandHandler::handleQuitCommand(Client* client, const std::vector<std::string>& tokens)
{
	std::string reason = "Quit: ";
	if (tokens.size() > 1)
	{
		reason += tokens[1];
	}

	std::string quitMessage = ":" + client->getNickname() + " QUIT :" + reason + "\r\n";

	std::map<std::string, Channel*>::iterator it;
	for (it = _server->getChannels().begin(); it != _server->getChannels().end(); ++it)
	{
		if (it->second->hasClient(client))
		{
			it->second->broadcast(quitMessage, client, _server);
			it->second->removeClient(client);
		}
	}

	_server->sendToClient(client->getFd(), "ERROR :" + reason + "\r\n");
	_server->disconnectClient(client->getFd());
}

void CommandHandler::handleErrorCommand(Client* client, const std::string &message)
{
	_server->sendToClient(client->getFd(), "ERROR :" + message + "\r\n");
	_server->disconnectClient(client->getFd());
}
