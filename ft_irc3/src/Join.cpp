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

void JoinHandler::handleJoinCommand(Client *client, const std::string &channelName, Server *server)
{
    std::map<std::string, Channel *> &channels = server->getChannels();
    Channel *channel;
    if (channels.find(channelName) == channels.end())
    {
        channel = new Channel(channelName);
        channels[channelName] = channel;
    }
    else
    {
        channel = channels[channelName];
    }

    channel->addClient(client);
    std::string joinMessage = ":" + client->getNickname() + " JOIN " + channelName + "\r\n";
    server->sendToClient(client->getFd(), joinMessage);

    std::string usersList = getUsersList(channel);
    server->sendToClient(client->getFd(), usersList);
}

std::string JoinHandler::getUsersList(Channel *channel)
{
    std::vector<Client *> clients = channel->getClients();
    std::string usersList = ":server 353 " + clients[0]->getNickname() + " = " + channel->getName() + " :";
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        usersList += (*it)->getNickname() + " ";
    }
    usersList += "\r\n:server 366 " + clients[0]->getNickname() + " " + channel->getName() + " :End of /NAMES list.\r\n";
    return usersList;
}
