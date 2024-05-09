/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:01:34 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/09 14:48:31 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, const std::string& password) : port(port), password(password), running(false) {}

Server::~Server() {
    // Nettoyage des ressources si nécessaire
    close(listener);  // Assurez-vous de fermer le socket d'écoute
    // Nettoyage des clients dynamiquement alloués, si vous ne l'avez pas déjà fait
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        delete *it;
    }
}

bool Server::initialize() {
    struct sockaddr_in server_addr;

    // Création du socket
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return false;
    }

    // Préparation de l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Liaison du socket
    if (bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
        close(listener);
        return false;
    }

    // Mise en écoute du socket
    if (listen(listener, 10) == -1) {  // backlog set to 10
        std::cerr << "Error listening: " << strerror(errno) << std::endl;
        close(listener);
        return false;
    }

    std::cout << "Server initialized and listening on port " << port << std::endl;
    return true;
}

void Server::run() {
    running = true;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    CommandHandler cmdHandler;

    while (running) {
        int client_sock = accept(listener, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock == -1) {
            std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
            continue;
        }

        std::cout << "Client connected: " << client_sock << std::endl;
        try {
            // Créez un nouvel objet Client avec des valeurs par défaut pour nickname et username
            Client* newClient = new Client(client_sock, "DefaultNickname", "DefaultUsername");
            std::string welcomeMsg = "Welcome to the IRC Server!\n";
            newClient->sendMessage(welcomeMsg);
            manageClient(newClient, cmdHandler);
        } catch (const std::exception& e) {
            std::cerr << "Exception handling client: " << e.what() << std::endl;
            close(client_sock);
        }
    }
}

void Server::manageClient(Client* client, CommandHandler& cmdHandler) {
    // Boucle pour recevoir les commandes du client
    while (running) {
        std::string command = client->receiveMessage();
        if (command.empty()) break; // Client disconnected
        cmdHandler.handleCommand(command, client);
    }
    delete client;  // Cleanup when client disconnects
}
