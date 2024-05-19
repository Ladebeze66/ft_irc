/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:41:35 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/19 15:14:50 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include "RPL.hpp"

class Client;

class Channel {
public:
    Channel(const std::string &name);
    ~Channel();

    const std::string &getName() const;
    void addClient(Client *client);
    void removeClient(Client *client);
    bool isEmpty() const;
    const std::vector<Client *> &getClients() const;
    void addOperator(Client *client);
    bool isOperator(Client *client) const;

private:
    std::string _name;
    std::vector<Client *> _clients;
    std::vector<Client *> _operators;
};

#endif
