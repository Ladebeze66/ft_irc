/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:15:13 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/21 19:44:24 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Utils.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "ClientManager.hpp"
#include "CommandHandler.hpp"
#include "AdditionalCommands.hpp"
#include "RPL.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>
#include <poll.h>

class Client;
class Channel;
class ClientManager;
class CommandHandler;
class AdditionalCommands;

class Server
{
public:
    Server(int port, const std::string &password);
    ~Server();
    void run();
    void log(const std::string &message, const std::string &color = "\033[0m");
    void sendToClient(int client_fd, const std::string &message);

	// Méthodes d'accès pour les canaux et les clients
    std::map<std::string, Channel *> &getChannels();
    std::map<int, Client *> &getClients();
	const std::string &getPassword() const;
	void broadcast(const std::string &message);
    Client* getClientByName(const std::string &name); // Ajoutez cette méthode
	void sendChannelListToClient(Client *client);
	void disconnectClient(int clientFd);
	bool MatchFd(const pollfd& pfd, int clientFd);
	void removePollFd(int clientFd);


protected:
    int _server_fd;
    int _port;
    std::string _password;
    std::map<int, Client *> _clients;
    std::map<std::string, Channel *> _channels;
    std::vector<struct pollfd> _poll_fds;
    ClientManager *_clientManager;
    CommandHandler *_commandHandler;

    friend class ClientManager;
    friend class CommandHandler;

private:
    void initServer();
	void handleServerCommands();
    void acceptClient();
    void removeClient(int client_fd);
};

#endif
