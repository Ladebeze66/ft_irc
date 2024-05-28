/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 18:30:07 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/28 14:17:28 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTMANAGER_HPP
#define CLIENTMANAGER_HPP

#include <map>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "CommandHandler.hpp"
#include "RPL.hpp"
#include "Utils.hpp"

#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <fstream>

class Server;
class Client;

class ClientManager
{
public:
	ClientManager(Server *server);
	void acceptClient();
	void handleClient(int client_fd);
	void removeClient(int client_fd);


private:
	Server *_server;
};

#endif
