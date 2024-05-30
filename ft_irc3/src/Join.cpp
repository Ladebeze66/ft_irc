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

/*void JoinHandler::handleJoinCommand(Client *client, const std::string &channelNames, Server *server)
{
    if (channelNames.empty()) {
        server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "JOIN"));
        return;
    }

    std::vector<std::string> channelList = split(channelNames, ",");
    for (std::vector<std::string>::iterator it = channelList.begin(); it != channelList.end(); ++it)
    {
        const std::string &channelName = *it;
        if (channelName.empty()) continue;

        std::map<std::string, Channel *> &channels = server->getChannels();
        Channel *channel;

        if (channels.find(channelName) == channels.end()) {
            channel = new Channel(channelName);
            channels[channelName] = channel;
            channel->addOperator(client); // Set client as operator for new channel
        } else {
            channel = channels[channelName];
        }

        if (channel->isBanned(client)) {
            server->sendToClient(client->getFd(), ERR_BANNEDFROMCHAN(client, channelName));
            continue;
        }

        if (channel->isFull()) {
            server->sendToClient(client->getFd(), ERR_CHANNELISFULL(client, channelName));
            continue;
        }

        if (channel->isInviteOnly() && !channel->isInvited(client)) {
            server->sendToClient(client->getFd(), ERR_INVITEONLYCHAN(client, channelName));
            continue;
        }

        channel->addClient(client);
        sendJoinSuccess(client, channel, server);
    }
}

void JoinHandler::sendJoinSuccess(Client *client, Channel *channel, Server *server)
{
    std::string joinMessage = ":" + client->getNickname() + " JOIN " + channel->getName() + "\r\n";
    channel->broadcast(joinMessage, client, server);

    if (!channel->getTopic().empty()) {
        server->sendToClient(client->getFd(), RPL_TOPIC(client, channel->getName(), channel->getTopic()));
        server->sendToClient(client->getFd(), RPL_TOPICWHOTIME(client, channel->getName(), channel->getTopicSetter(), channel->getTopicTime()));
    } else {
        server->sendToClient(client->getFd(), RPL_NOTOPIC(client, channel->getName()));
    }

    std::string usersList = getUsersList(channel);
    server->sendToClient(client->getFd(), usersList);
}

std::string JoinHandler::getUsersList(Channel *channel)
{
    std::vector<Client *> clients = channel->getClients();
    std::string users;
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (channel->isOperator(*it))
            users += "@";
        users += (*it)->getNickname() + " ";
    }

    std::ostringstream oss;
    oss << RPL_NAMREPLY(clients[0], channel->getName(), users);
    oss << RPL_ENDOFNAMES(clients[0], channel->getName());
    return oss.str();
}*/

void JoinHandler::handleJoinCommand(Client *client, const std::string &channelNames, Server *server)
{
    std::map<std::string, Channel *> &channels = server->getChannels();
    Channel *channel;

    std::vector<std::string> lst = split(channelNames, ",");
    for (std::vector<std::string>::iterator itl = lst.begin(); itl != lst.end(); ++itl)
    {
        const std::string &channelName = *itl;
        std::cout << "hjoincmd .. " << channelName << std::endl;

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

        channel->addClient(client);
		sendJoinSuccess(client, channel, server);
	}
}

void JoinHandler::sendJoinSuccess(Client *client, Channel *channel, Server *server)
{
        std::vector<Client *> clients = channel->getClients();
        for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
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

        std::string usersList = getUsersList(channel);
        server->sendToClient(client->getFd(), usersList);
    }


std::string JoinHandler::getUsersList(Channel *channel)
{
    std::vector<Client *> clients = channel->getClients();
    std::string users;
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
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
