/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:41:35 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/06/04 16:12:23 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <ctime>

#include "RPL.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Server;
class Client;

class Channel
{
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
		void removeOperator(Client *client);  // Ajouté
		bool hasClient(Client *client) const;
		void broadcast(const std::string &message, Client *_client, Server *_server);
		bool isBanned(Client *client) const;
		bool isFull() const;
		bool isInviteOnly() const;
		bool isInvited(Client *client) const;
		bool checkKey(const std::string &key) const;
		const std::string &getTopic() const;
		const std::string &getTopicSetter() const;
		time_t getTopicTime() const;
		void setTopic(const std::string &topic, const std::string &setter);
		std::string getKey();

		void setClientLimit(size_t limit);
		size_t getClientLimit() const;
		void setInviteOnly(bool inviteOnly);
		void setKey(const std::string &key);
		void setTopicProtection(bool protection);
		std::string getModes() const;
		bool getTopicProtection() const;
		void addInvitedClient(Client* client);

	private:
		std::string _name;
		std::vector<Client *> _clients;
		std::vector<Client *> _operators;
		std::set<Client *> _bannedClients;
		std::set<Client *> _invitedClients;
		std::string _key;
		std::string _topic;
		std::string _topicSetter;
		time_t _topicTime;
		size_t _clientLimit;
		bool _inviteOnly;
		bool _topicProtection;

};

#endif
