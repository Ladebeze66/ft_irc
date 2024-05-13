/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:08:13 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/12 17:56:26 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <algorithm>

Channel::Channel(const std::string& name) : name(name) {}

void Channel::addClient(Client* client) {
    std::cout << "Adding client to channel " << name << std::endl;
    clients.push_back(client);
    std::cout << "Client added. Total clients: " << clients.size() << std::endl;
}
void Channel::removeClient(Client* client) {
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
}

void Channel::broadcastMessage(const std::string& message, Client* sender) {
    std::cout << "Broadcasting message in channel " << name << " from " << sender->getNickname() << std::endl;
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i] != sender) {
            std::cout << "Sending message to " << clients[i]->getNickname() << std::endl;
            clients[i]->sendMessage(message);
        }
    }
}
