/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeWhoHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 16:09:20 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/17 18:24:34 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ModeWhoHandler.hpp"
#include "Channel.hpp"
#include <sstream>
#include <iostream>

// Couleurs ANSI pour les logs
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"

ModeWhoHandler::ModeWhoHandler(Server *server)
    : _server(server)
{
}

void ModeWhoHandler::handleModeCommand(Client *client, const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd, channelName, mode, user;
    iss >> cmd >> channelName >> mode >> user;

    std::map<std::string, Channel *> &channels = _server->getChannels();
    if (channels.find(channelName) == channels.end())
    {
        _server->sendToClient(client->getFd(), ":server 403 " + client->getNickname() + " " + channelName + " :No such channel\r\n");
        return;
    }

    Channel *channel = channels[channelName];
    if (mode == "+o")
    {
        if (client->isOperator())
        {
            Client *targetClient = _server->getClientByName(user);
            if (targetClient)
            {
                channel->addOperator(targetClient);
                _server->sendToClient(client->getFd(), ":" + client->getNickname() + " MODE " + channelName + " +o " + user + "\r\n");
                _server->log(GREEN "Client " + user + " is now an operator in channel " + channelName + RESET, GREEN);
            }
            else
            {
                _server->sendToClient(client->getFd(), ":server 401 " + client->getNickname() + " " + user + " :No such nick/channel\r\n");
            }
        }
        else
        {
            _server->sendToClient(client->getFd(), ":server 482 " + client->getNickname() + " " + channelName + " :You're not channel operator\r\n");
        }
    }
}

void ModeWhoHandler::handleWhoCommand(Client *client, const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd, channelName;
    iss >> cmd >> channelName;

    std::map<std::string, Channel *> &channels = _server->getChannels();
    if (channels.find(channelName) == channels.end())
    {
        _server->sendToClient(client->getFd(), ":server 403 " + client->getNickname() + " " + channelName + " :No such channel\r\n");
        return;
    }

    Channel *channel = channels[channelName];
    std::vector<Client *> channelClients = channel->getClients();

    for (size_t i = 0; i < channelClients.size(); ++i)
    {
        std::stringstream whoMsg;
        whoMsg << ":server 352 " << client->getNickname() << " " << channelName << " " << channelClients[i]->getNickname() << " "
               << channelClients[i]->getUser() << " " << _server->getPassword() << " " << channelClients[i]->getRealName() << "\r\n";
        _server->sendToClient(client->getFd(), whoMsg.str());
    }

    std::stringstream endOfWhoMsg;
    endOfWhoMsg << ":server 315 " << client->getNickname() << " " << channelName << " :End of /WHO list.\r\n";
    _server->sendToClient(client->getFd(), endOfWhoMsg.str());
}
