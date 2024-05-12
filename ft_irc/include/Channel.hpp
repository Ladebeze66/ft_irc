/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:07:50 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/11 16:20:58 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include <vector>
#include <string>
#include <algorithm>

class Client;


class Channel {
public:
    Channel(const std::string& name);
    void addClient(Client* client);
    void removeClient(Client* client);
    void broadcastMessage(const std::string& message, Client* sender);

private:
    std::string name;
    std::vector<Client*> clients;
};

#endif
