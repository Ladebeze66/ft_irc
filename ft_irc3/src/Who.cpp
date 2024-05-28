/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 16:09:20 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/21 18:03:56 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Who.hpp"

WhoHandler::WhoHandler(Server *server)
    : _server(server)
{
}

void WhoHandler::handleWhoCommand(Client *client, const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd, mask;
    iss >> cmd >> mask;

    std::map<std::string, Channel *> &channels = _server->getChannels();
    if (channels.find(mask) != channels.end())
    {
        Channel *channel = channels[mask];
        std::vector<Client *> channelClients = channel->getClients();

        for (size_t i = 0; i < channelClients.size(); ++i)
        {
            _server->sendToClient(client->getFd(), RPL_WHOREPLY(mask, channelClients[i]));
        }
    }
    else
    {
        Client *targetClient = _server->getClientByName(mask);
        if (targetClient)
        {
            _server->sendToClient(client->getFd(), RPL_WHOREPLY("*", targetClient));
        }
    }

    _server->sendToClient(client->getFd(), RPL_ENDOFWHO(client->getFd(), mask));
}

void WhoHandler::handleWhoisCommand(Client *client, const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd, target;
    iss >> cmd >> target;

    if (target.empty())
    {
        _server->sendToClient(client->getFd(), ERR_NONICKNAMEGIVEN(client));
        return;
    }

    Client *targetClient = _server->getClientByName(target);
    if (!targetClient)
    {
        _server->sendToClient(client->getFd(), ERR_NOSUCHNICK(client->getFd(), target));
        return;
    }

    _server->sendToClient(client->getFd(), RPL_WHOISUSER(client->getFd(), targetClient));
    _server->sendToClient(client->getFd(), RPL_WHOISSERVER(client->getFd(), target, "IRC server info"));
    _server->sendToClient(client->getFd(), RPL_ENDOFWHOIS(client->getFd(), target));
}
