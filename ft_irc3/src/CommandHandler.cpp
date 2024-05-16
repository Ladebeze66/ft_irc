/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 18:26:34 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/16 19:34:36 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

CommandHandler::CommandHandler(Server *server)
    : _server(server)
{
}

void CommandHandler::handleCommand(Client *client, const std::string &command)
{
    std::vector<std::string> tokens = split(command, " \n\r\t");

    if (tokens.empty())
    {
        return;
    }

    std::string commandType = tokens[0];

    if (commandType == "PASS")
    {
        if (tokens.size() > 1 && tokens[1] == _server->_password)
        {
            client->setPassword(tokens[1]);
            _server->sendToClient(client->getFd(), ":server 001 " + client->getNickname() + " :Password accepted\r\n");
            _server->log("Client " + client->getNickname() + " provided correct password.", GREEN);
        }
        else
        {
            _server->sendToClient(client->getFd(), ":server 464 " + client->getNickname() + " :Invalid password\r\n");
            _server->log("Client " + client->getNickname() + " failed authentication password.", RED);
        }
    }
    else if (commandType == "NICK")
    {
        if (tokens.size() > 1)
        {
            client->setNickname(tokens[1]);
            std::stringstream ss;
            ss << "Client " << client->getFd() << " Nickname: " << tokens[1];
            _server->log(ss.str(), GREEN);
            _server->sendToClient(client->getFd(), ":" + tokens[1] + " NICK " + tokens[1] + "\r\n");
        }
    }
    else if (commandType == "USER")
    {
        if (tokens.size() > 4)
        {
            client->setUser(tokens[1]);
            client->setRealName(tokens[4].substr(1)); // remove leading ':'
            if (client->getPassword() == _server->_password && !client->getNickname().empty())
            {
                client->authenticate();
                _server->sendToClient(client->getFd(), ":server 001 " + client->getNickname() + " :Welcome to the IRC server\r\n");
                _server->log("Client " + client->getNickname() + " authenticated.", GREEN);
            }
        }
    }
    else
    {
        if (!client->isAuthenticated())
        {
            _server->sendToClient(client->getFd(), ":server 451 " + client->getNickname() + " :Please provide the password first\r\n");
            _server->log("Client " + client->getNickname() + " failed authentication.", RED);
        }
        else
        {
            processCommand(client, command);
        }
    }
    std::cout << "Client " << client->getFd() << " " << client->getNickname() << " " << client->getUser() << " " << client->getPassword() << " " << client->getRealName() << std::endl;
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
    else if (command.find("NICK") == 0)
    {
        handleNickCommand(_server, client, command);
    }
    else if (command.find("PRIVMSG") == 0)
    {
        handlePrivmsgCommand(_server, client, command);
    }
    else
    {
        std::stringstream ss;
        ss << ":server 421 " << client->getNickname() << " " << command << " :Unknown command\r\n";
        _server->sendToClient(client->getFd(), ss.str());
        _server->log("Message from client " + client->getNickname() + ": " + command, MAGENTA);
    }
}

void CommandHandler::handleJoinCommand(Client *client, const std::string &channelName)
{
    if (_server->getChannels().find(channelName) == _server->getChannels().end())
    {
        _server->getChannels()[channelName] = new Channel(channelName);
        _server->log("Channel created: " + channelName, GREEN);
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
