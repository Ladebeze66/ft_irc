/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:01:12 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/13 19:00:18 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <cerrno>
#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <fcntl.h>  // Include this for fcntl and O_NONBLOCK
#include <sys/epoll.h>


#include "color.hpp"
#include "Client.hpp"
#include "CommandHandler.hpp"

class Client;
class CommandHandler;

class Server
{
public:
	Server(int port, const std::string& password);
	~Server();
	void run();
	bool initialize();
	void closeClient(int sockfd);

private:
	int port;     // Port d'écoute
	std::string password; // Mot de passe pour les clients se connectant

	bool running; // État d'exécution du serveur
	int listener; // Socket d'écoute du serveur
	int fdmax;
	int epoll_fd; // Descripteur pour l'instance epoll
	static const int MAX_EVENTS = 10; // Nombre maximal d'événements traités à chaque appel à epoll_wait
	fd_set master_set; // Ensemble principal des descripteurs de fichiers pour select
	std::map<int, Client*> clients; // Mapping de socket à Client
	std::map<int, CommandHandler*> handlers; // CommandHandler pour chaque client

	void acceptNewClient();
	bool handleClientActivity(int sockfd);
	bool getlineFromClient(Client* client, std::string& line);
	void handleIRCMessages(Client* client, CommandHandler& cmdHandler);
	bool processInitialCommands(Client* client);
	void handleNickAndUserCommands(const std::string& line, size_t passEnd, Client* client);
	void setNonBlocking(int sockfd);
};

#endif // SERVER_HPP
