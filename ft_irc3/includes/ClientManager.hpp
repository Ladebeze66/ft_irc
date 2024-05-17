/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 18:30:07 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/17 18:51:44 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTMANAGER_HPP
#define CLIENTMANAGER_HPP

#include <map>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include <poll.h>
#include "CommandHandler.hpp"
#include "Utils.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <fcntl.h>

class Server;

class ClientManager
{
public:
    ClientManager(Server *server);
    void acceptClient();
	void broadcastChannelList(Client *client);
    void handleClient(int client_fd);
    void removeClient(int client_fd);


private:
    Server *_server;
};

#endif
