/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:01:12 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/10 14:10:44 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <pthread.h>

#include "Client.hpp"  // Assurez-vous que Client est inclus avant Server
#include "CommandHandler.hpp"

class Server;  // Déclaration anticipée de Server si nécessaire

// Structure pour passer des données aux threads
struct ClientContext {
    Client* client;
    Server* server;
    CommandHandler* cmdHandler;

    ClientContext(Client* c, Server* s, CommandHandler* ch) : client(c), server(s), cmdHandler(ch) {}
};
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
    std::vector<pthread_t> threads; // List of threads for client management

    static void *manageClient(void *clientContext); // Static to be compatible with pthread
};

#endif // SERVER_HPP


