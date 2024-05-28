/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 19:51:31 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/28 13:07:53 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Join.hpp"

void JoinHandler::handleJoinCommand(Client *client, const std::string &channelName, Server *server)
{
    if (channelName.empty()) {
        server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "JOIN"));
        return;
    }

    std::map<std::string, Channel *> &channels = server->getChannels();
    Channel *channel;

    if (channels.find(channelName) == channels.end()) {
        channel = new Channel(channelName);
        channels[channelName] = channel;
    } else {
        channel = channels[channelName];
    }

    if (channel->isBanned(client)) {
        server->sendToClient(client->getFd(), ERR_BANNEDFROMCHAN(client, channelName));
        return;
    }

    if (channel->isFull()) {
        server->sendToClient(client->getFd(), ERR_CHANNELISFULL(client, channelName));
        return;
    }

    if (channel->isInviteOnly() && !channel->isInvited(client)) {
        server->sendToClient(client->getFd(), ERR_INVITEONLYCHAN(client, channelName));
        return;
    }

    channel->addClient(client);
    sendJoinSuccess(client, channel, server);
}

void JoinHandler::sendJoinSuccess(Client *client, Channel *channel, Server *server)
{
    std::string joinMessage = ":" + client->getNickname() + " JOIN " + channel->getName() + "\r\n";
    server->sendToClient(client->getFd(), joinMessage);

    if (!channel->getTopic().empty()) {
        server->sendToClient(client->getFd(), RPL_TOPIC(client, channel->getName(), channel->getTopic()));
        server->sendToClient(client->getFd(), RPL_TOPICWHOTIME(client, channel->getName(), channel->getTopicSetter(), channel->getTopicTime()));
    }

    std::string usersList = getUsersList(channel);
    server->sendToClient(client->getFd(), usersList);

    channel->broadcast(joinMessage, client, server);
}

std::string JoinHandler::getUsersList(Channel *channel)
{
    std::vector<Client *> clients = channel->getClients();
    std::string users;
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
        users += (*it)->getNickname() + " ";
    }

    std::ostringstream oss;
    oss << RPL_NAMREPLY(clients[0], channel->getName(), users);
    oss << RPL_ENDOFNAMES(clients[0], channel->getName());
    return oss.str();
}
