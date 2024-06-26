/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:42:57 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/28 13:06:02 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &name)
    : _name(name), _clients(), _operators(), _bannedClients(), _invitedClients(), _key(""), _topic(""), _topicSetter(""), _topicTime(0), _clientLimit(50), _inviteOnly(false) {}

Channel::~Channel() {}

const std::string &Channel::getName() const
{
    return _name;
}

void Channel::addClient(Client *client)
{
    _clients.push_back(client);
}

void Channel::removeClient(Client *client)
{
    _clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());
}

bool Channel::isEmpty() const
{
    return _clients.empty();
}

const std::vector<Client *> &Channel::getClients() const
{
    return _clients;
}

void Channel::addOperator(Client *client)
{
    _operators.push_back(client);
}

bool Channel::isOperator(Client *client) const
{
    return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}

bool Channel::hasClient(Client *client) const
{
    return std::find(_clients.begin(), _clients.end(), client) != _clients.end();
}

void Channel::broadcast(const std::string &message, Client *_client, Server *_server)
{
    for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (*it != _client)
        {
            // Send message to each client except the sender
            _server->sendToClient((*it)->getFd(), message);
        }
    }
}

bool Channel::isBanned(Client *client) const
{
    return _bannedClients.find(client) != _bannedClients.end();
}

bool Channel::isFull() const
{
    return _clients.size() >= _clientLimit;
}

bool Channel::isInviteOnly() const
{
    return _inviteOnly;
}

bool Channel::isInvited(Client *client) const
{
    return _invitedClients.find(client) != _invitedClients.end();
}

bool Channel::checkKey(const std::string &key) const
{
    return _key.empty() || _key == key;
}

const std::string &Channel::getTopic() const
{
    return _topic;
}

const std::string &Channel::getTopicSetter() const
{
    return _topicSetter;
}

time_t Channel::getTopicTime() const
{
    return _topicTime;
}

void Channel::setTopic(const std::string &topic, const std::string &setter)
{
    _topic = topic;
    _topicSetter = setter;
    _topicTime = std::time(NULL);
}


