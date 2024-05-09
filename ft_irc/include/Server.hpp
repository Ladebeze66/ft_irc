/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:01:12 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/09 14:39:02 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include "Client.hpp"
#include "CommandHandler.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <cerrno>

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();
    bool initialize();
    void run();

private:
    int listener;               // Socket descriptor for the server
    int port;                   // Port number to bind the server
    std::string password;       // Server password for client connections
    bool running;               // Server running status
    std::vector<Client*> clients; // List of connected clients

    void manageClient(Client* client, CommandHandler& cmdHandler);
};

#endif // SERVER_HPP
