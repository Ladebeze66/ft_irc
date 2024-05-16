/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:42:57 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/15 12:54:40 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"
#include <algorithm>

Channel::Channel(const std::string &name)
	: _name(name)
{
}

Channel::~Channel()
{
}

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
