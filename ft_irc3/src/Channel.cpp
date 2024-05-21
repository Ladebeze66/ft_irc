/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:42:57 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/21 19:27:42 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &name) : _name(name) {}

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

void Channel::broadcast(const std::string &message, Client *_client, Server *server)
{
    for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (*it != _client)
        {
            // Send message to each client except the sender
            server->sendToClient((*it)->getFd(), message);
        }
    }
}
