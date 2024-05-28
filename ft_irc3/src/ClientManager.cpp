/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 18:32:23 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/28 14:32:28 by fgras-ca         ###   ########.fr       */
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
		_server->log("Failed to accept client.", RED);
		return;
	}

	Client *newClient = new Client(client_fd, "", "", "", "", "");   // Fournir six arguments
	_server->_clients[client_fd] = newClient;
	struct pollfd client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN;
	_server->_poll_fds.push_back(client_pollfd);

	std::stringstream ss;
	ss << "Client attempting connection: " << client_fd;
	_server->log(ss.str(), YELLOW);
}

void ClientManager::handleClient(int client_fd)
{
	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));
	int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

	if (bytes_received <= 0)
	{
		std::ostringstream oss;
		oss << "Client disconnected: " << client_fd;
		_server->log(oss.str(), RED);
		removeClient(client_fd);
		return;
	}

	std::string message(buffer, bytes_received);
	std::ostringstream oss;
	oss << "Received from client " << client_fd << ": " << message;
	_server->log(oss.str(), BLUE);

	Client* client = _server->getClients()[client_fd];
	if (!client)
	{
		std::ostringstream oss;
		oss << "Client not found for fd: " << client_fd;
		_server->log(oss.str(), RED);
		return;
	}

	std::istringstream message_stream(message);
	std::string line;

	while (std::getline(message_stream, line))
	{
		// Remove trailing '\r' and '\n'
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

		if (!line.empty())
		{
			std::ostringstream oss;
			oss << "Processing command from client " << client_fd << ": " << line;
			_server->log(oss.str(), BLUE);
			_server->_commandHandler->handleCommand(client, line);
		}
	}
}


void ClientManager::removeClient(int clientFd)
{
	Client* client = _server->_clients[clientFd];
	if (client)
	{
		// Log the nickname being freed
		_server->log("Removing client: " + client->getNickname(), YELLOW);

		std::map<std::string, Channel*>::iterator it = _server->_channels.begin();
		while (it != _server->_channels.end())
		{
			it->second->removeClient(client);
			if (it->second->isEmpty())
			{
				delete it->second;
				std::map<std::string, Channel*>::iterator it_to_delete = it++;
				_server->_channels.erase(it_to_delete);
			}
			else
			{
				++it;
			}
		}
		delete client;
		_server->_clients.erase(clientFd);
	}

	std::vector<struct pollfd>::iterator it_poll = _server->_poll_fds.begin();
	while (it_poll != _server->_poll_fds.end())
	{
		if (it_poll->fd == clientFd)
		{
			_server->_poll_fds.erase(it_poll);
			break;
		}
		++it_poll;
	}

	std::stringstream ss;
	ss << "Client disconnected: " << clientFd;
	_server->log(ss.str(), YELLOW);
}
