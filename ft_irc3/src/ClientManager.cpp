/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 18:32:23 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/16 16:25:50 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientManager.hpp"

ClientManager::ClientManager(Server *server)
    : _server(server)
{
}

void ClientManager::acceptClient()
{
    int client_fd = accept(_server->_server_fd, NULL, NULL);
    if (client_fd < 0)
    {
        _server->log("Accept failed.", RED);
        return;
    }
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    _server->_clients[client_fd] = new Client(client_fd);
    struct pollfd client_pollfd;
    client_pollfd.fd = client_fd;
    client_pollfd.events = POLLIN;
    _server->_poll_fds.push_back(client_pollfd);

    std::stringstream ss;
    ss << "Client connected: " << client_fd;
    _server->log(ss.str(), GREEN);
    _server->sendToClient(client_fd, ":server NOTICE * :Connected to the server\r\n");  // Debug message
}

void ClientManager::handleClient(int client_fd)
{
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0)
    {
        removeClient(client_fd);
        return;
    }

    std::string message(buffer);
    std::stringstream ss;
    ss << "Received from client " << client_fd << ": " << message;
    _server->log(ss.str(), BLUE);

    Client *client = _server->_clients[client_fd];
    std::istringstream message_stream(message);
    std::string line;

    while (std::getline(message_stream, line))
    {
        // Suppression des caractères de fin de ligne
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

        _server->_commandHandler->handleCommand(client, line);
    }
}

void ClientManager::removeClient(int client_fd)
{
    Client *client = _server->_clients[client_fd];
    if (client)
    {
        // Retirer le client de tous les canaux auxquels il appartient
        std::map<std::string, Channel *>::iterator it = _server->_channels.begin();
        while (it != _server->_channels.end())
        {
            it->second->removeClient(client);
            if (it->second->isEmpty())
            {
                delete it->second;
                std::map<std::string, Channel *>::iterator it_to_delete = it++;
                _server->_channels.erase(it_to_delete);
            }
            else
            {
                ++it;
            }
        }
        delete client;
        _server->_clients.erase(client_fd);
    }

    std::vector<struct pollfd>::iterator it_poll = _server->_poll_fds.begin();
    while (it_poll != _server->_poll_fds.end())
    {
        if (it_poll->fd == client_fd)
        {
            _server->_poll_fds.erase(it_poll);
            break;
        }
        ++it_poll;
    }

    std::stringstream ss;
    ss << "Client disconnected: " << client_fd;
    _server->log(ss.str(), YELLOW);
}
