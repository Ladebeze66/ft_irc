/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:17:12 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/21 14:18:03 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, const std::string &password)
	: _port(port), _password(password), _clientManager(new ClientManager(this)), _commandHandler(new CommandHandler(this))
{
	initServer();
}

Server::~Server()
{
	delete _clientManager;
	delete _commandHandler;

	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		delete it->second;
	}
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		delete it->second;
	}
	close(_server_fd);
}

void Server::initServer()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == -1)
	{
		log("Failed to create socket.", RED);
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		log("Failed to set socket options.", RED);
		close(_server_fd);
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_addr;
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(_port);

	if (bind(_server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		log("Failed to bind socket.", RED);
		close(_server_fd);  // Close the socket if bind fails
		exit(EXIT_FAILURE);
	}

	if (listen(_server_fd, SOMAXCONN) == -1)
	{
		log("Failed to listen on socket.", RED);
		close(_server_fd);  // Close the socket if listen fails
		exit(EXIT_FAILURE);
	}

	log("Server initialized.", GREEN);
}

void Server::run()
{
	struct pollfd server_pollfd;
	server_pollfd.fd = _server_fd;
	server_pollfd.events = POLLIN;
	server_pollfd.revents = 0;
	_poll_fds.push_back(server_pollfd);

	// Ajout de l'entrée pour les commandes serveur
	struct pollfd stdin_pollfd;
	stdin_pollfd.fd = STDIN_FILENO;
	stdin_pollfd.events = POLLIN;
	stdin_pollfd.revents = 0;
	_poll_fds.push_back(stdin_pollfd);

	while (true)
	{
		int poll_count = poll(&_poll_fds[0], _poll_fds.size(), -1);
		if (poll_count == -1)
		{
			log("Poll error.", RED);
			exit(EXIT_FAILURE);
		}

		for (size_t i = 0; i < _poll_fds.size(); ++i)
		{
			if (_poll_fds[i].revents & POLLIN)
			{
				if (_poll_fds[i].fd == _server_fd)
				{
					_clientManager->acceptClient();
				}
				else if (_poll_fds[i].fd == STDIN_FILENO)
				{
					handleServerCommands();
				}
				else
				{
					_clientManager->handleClient(_poll_fds[i].fd);
				}
			}
		}
	}
}


std::map<std::string, Channel *> &Server::getChannels()
{
	return _channels;
}

const std::string &Server::getPassword() const
{
	return _password;
}

void Server::handleServerCommands()
{
	std::string command;
	std::getline(std::cin, command);

	if (command == "quit")
	{
		log("Server shutting down.", YELLOW);
		exit(EXIT_SUCCESS);
	}
	else if (command == "channels")
	{
		log("Listing all channels:", BLUE);
		for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			log(it->first, BLUE);
		}
	}
	else if (command == "clients")
	{
		log("Listing all clients:", BLUE);
		for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			log(it->second->getNickname(), BLUE);
		}
	}
	else
	{
		log("Unknown server command.", RED);
	}
}

void Server::log(const std::string &message, const std::string &color)
{
	std::cout << color << message << "\033[0m" << std::endl;
}

void Server::sendToClient(int client_fd, const std::string &message)
{
	int result = send(client_fd, message.c_str(), message.length(), 0);
	if (result < 0)
	{
		std::stringstream ss;
		ss << "Failed to send message to client " << client_fd;
		log(ss.str(), RED);
	}
	else
	{
		std::stringstream ss;
		ss << "Sent message to client " << client_fd << ": " << message;
		log(ss.str(), BLUE);
	}
}

void Server::broadcast(const std::string &message)
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		sendToClient(it->first, message);
	}
}

Client* Server::getClientByName(const std::string &name)
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == name)
		{
			return it->second;
		}
	}
	return NULL; // Remplacez nullptr par NULL
}

void Server::sendChannelListToClient(Client *client)
{
	std::map<std::string, Channel *> &channels = getChannels();
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		sendToClient(client->getFd(), RPL_LIST(client->getFd(), it->first, it->second->getClients().size(), "Existing channel"));
	}
	sendToClient(client->getFd(), RPL_LISTEND(client->getFd()));
}

void Server::disconnectClient(int clientFd)
{
    Client* client = _clients[clientFd];
    if (client)
    {
        log("Disconnecting client: " + client->getNickname(), YELLOW);
        _clients.erase(clientFd);
        delete client;
    }

    struct MatchFd
    {
        int fd;
        MatchFd(int fd) : fd(fd) {}
        bool operator()(const pollfd& pfd) const { return pfd.fd == fd; }
    };

    _poll_fds.erase(std::remove_if(_poll_fds.begin(), _poll_fds.end(), MatchFd(clientFd)), _poll_fds.end());

    std::ostringstream oss;
    oss << "Client disconnected: " << clientFd;
    log(oss.str(), YELLOW);
}
