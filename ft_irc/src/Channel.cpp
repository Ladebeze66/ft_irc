/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:08:13 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/09 14:38:23 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <algorithm>

Channel::Channel(const std::string& name) : name(name) {}

void Channel::addClient(Client* client) {
    clients.push_back(client);
}

void Channel::removeClient(Client* client) {
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
}

void Channel::broadcastMessage(const std::string& message, Client* sender) {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (*it != sender) {  // Ensure we don't send the message back to the sender
            (*it)->sendMessage(message);
        }
    }
}
