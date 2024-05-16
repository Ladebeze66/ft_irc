/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:41:35 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/15 12:48:40 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>

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

private:
	std::string _name;
	std::vector<Client *> _clients;
};

#endif
