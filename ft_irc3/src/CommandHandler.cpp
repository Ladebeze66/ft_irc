/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 18:26:34 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/21 14:19:19 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

CommandHandler::CommandHandler(Server *server)
    : _server(server)
{
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
    } else {
        if (!client->isAuthenticated()) {
            _server->sendToClient(client->getFd(), ERR_NOTREGISTERED(client));
            _server->log("Client " + client->getNickname() + " attempted to send a command before registering.", RED);
        } else {
            processCommand(client, command);
        }
    }
    std::cout << "Client " << client->getFd() << " " << client->getNickname() << " " << client->getUser() << " " << client->getPassword() << " " << client->getRealName() << std::endl;
}

void CommandHandler::handleCapCommand(Client* client, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2)
    {
        _server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client->getFd(), "CAP"));
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
            _server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client->getFd(), "CAP"));
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
        _server->sendToClient(client->getFd(), ERR_UNKNOWNCOMMAND(client->getFd(), "CAP"));
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
    
    client->setNickname(newNick);

    _server->sendToClient(client->getFd(), ":" + newNick + " NICK " + newNick + "\r\n");

    std::ostringstream oss;
    oss << "Client " << client->getFd() << " changed nickname to " << newNick;
    _server->log(oss.str(), GREEN);
}

void CommandHandler::handleUser(Client* client, const std::vector<std::string>& tokens)
{
    // Set the user and realname fields
    client->setUser(tokens[1]);
    std::string realname = tokens[4];
    if (realname[0] == ':')
	{
        realname = realname.substr(1); // Remove leading ':'
    }
    client->setRealName(realname);

    // Log the values for debugging
    std::ostringstream logMsg;
    logMsg << "Client " << client->getFd() << ": USER command set username to " << tokens[1] << " and real name to " << realname;
    _server->log(logMsg.str(), BLUE);

    // Authenticate if password and nickname are already set
    if (client->getPassword() == _server->_password && !client->getNickname().empty())
	{
        client->authenticate();
        sendWelcomeMessages(client, _server);
        _server->log("Client " + client->getNickname() + " authenticated successfully.", GREEN);
    }
	else
	{
        std::ostringstream authFailMsg;
        authFailMsg << "Client " << client->getFd() << ": USER command failed - authentication conditions not met.";
        _server->log(authFailMsg.str(), RED);
    }
}



void CommandHandler::processCommand(Client *client, const std::string &command)
{
    if (command.find("JOIN") == 0)
    {
        std::string channelName = command.substr(5);
        handleJoinCommand(client, channelName);
    }
    else if (command.find("PART") == 0)
    {
        handlePartCommand(_server, client, command);
    }
    /*else if (command.find("NICK") == 0)
    {
        handleNickCommand(_server, client, command);
    }*/
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
	else if (command.find("PING") == 0)
    {
        std::vector<std::string> tokens = split(command, " ");
        handlePingCommand(client, tokens);
    }
	else if (command.find("LIST") == 0)
    {
        broadcastChannelList(client, _server);
    }
    else
    {
        _server->sendToClient(client->getFd(), ERR_UNKNOWNCOMMAND(client, command));
        _server->log("Message from client " + client->getNickname() + ": " + command, MAGENTA);
    }
}

void CommandHandler::handleJoinCommand(Client *client, const std::string &channelName)
{
    if (_server->getChannels().find(channelName) == _server->getChannels().end())
    {
        _server->getChannels()[channelName] = new Channel(channelName);
        _server->log("Channel created: " + channelName, GREEN);

        // Notifier tous les clients de la création du canal
        _server->broadcast(":server NOTICE * :New channel created: " + channelName + "\r\n");
    }
    Channel *channel = _server->getChannels()[channelName];
    channel->addClient(client);

    std::stringstream joinMsg;
    joinMsg << ":" << client->getNickname() << " JOIN :" << channelName << "\r\n";
    _server->sendToClient(client->getFd(), joinMsg.str());

    std::string usersList = getUsersList(channel);
    _server->sendToClient(client->getFd(), usersList);

    std::stringstream endOfNamesMsg;
    endOfNamesMsg << ":server 366 " << client->getNickname() << " " << channelName << " :End of /NAMES list.\r\n";
    _server->sendToClient(client->getFd(), endOfNamesMsg.str());

    std::stringstream ss;
    ss << "Client " << client->getNickname() << " joined channel " << channelName;
    _server->log(ss.str(), MAGENTA);
}

std::string CommandHandler::getUsersList(Channel *channel)
{
    std::vector<Client *> clients = channel->getClients();
    std::stringstream ss;
    ss << ":server 353 " << clients[0]->getNickname() << " = " << channel->getName() << " :";
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (i > 0)
            ss << " ";
        ss << clients[i]->getNickname();
    }
    ss << "\r\n";
    return ss.str();
}

void CommandHandler::handlePingCommand(Client* client, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2) {
        _server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "PING"));
        return;
    }

    std::string token = tokens[1];
    _server->sendToClient(client->getFd(), RPL_PONG(token));
}
