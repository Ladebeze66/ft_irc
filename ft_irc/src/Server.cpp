/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:01:34 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/10 14:13:02 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Server.cpp
#include "Server.hpp"

Server::Server(int port, const std::string& password) : port(port), password(password), running(false) {}

Server::~Server() {
    for (size_t i = 0; i < threads.size(); ++i) {
        if (threads[i]) pthread_join(threads[i], NULL);
    }
    close(listener);
}

bool Server::initialize() {
    struct sockaddr_in server_addr;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return false;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
        close(listener);
        return false;
    }

    if (listen(listener, 10) == -1) {
        std::cerr << "Error listening: " << strerror(errno) << std::endl;
        close(listener);
        return false;
    }

    std::cout << "Server initialized and listening on port " << port << std::endl;
    running = true;
    return true;
}

void Server::run() {
    if (!running) {
        std::cerr << "Server not initialized or already running." << std::endl;
        return;
    }

    CommandHandler cmdHandler;
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(listener, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock == -1) {
            std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
            continue;
        }

        Client* newClient = new Client(client_sock, "DefaultNickname", "DefaultUsername");
        ClientContext* context = new ClientContext(newClient, this, &cmdHandler);
        pthread_t thread;
        pthread_create(&thread, NULL, &Server::manageClient, context);
        pthread_detach(thread);
        threads.push_back(thread);
    }
}

void* Server::manageClient(void *clientContext) {
    ClientContext* ctx = static_cast<ClientContext*>(clientContext);
    Client* client = ctx->client;
    Server* server = ctx->server;

    client->sendMessage("Welcome to the IRC Server!\n");
    client->sendMessage("Password required:\n");

    std::string passwordInput = client->receiveMessage();
    std::cout << "Received password: [" << passwordInput << "]" << std::endl;

    if (passwordInput != server->password) {
        client->sendMessage("Incorrect password.\n");
        std::cout << "Password mismatch." << std::endl;
        close(client->getSocket());
        delete client;
        delete ctx;
        return NULL;
    }

    client->sendMessage("Password correct.\n");

    std::string command;
    while ((command = client->receiveMessage()) != "") {
        ctx->cmdHandler->handleCommand(command, client);
        if (command == "QUIT") break;
    }

    close(client->getSocket());
    delete client;
    delete ctx;
    return NULL;
}








