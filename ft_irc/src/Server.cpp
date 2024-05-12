/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:01:34 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/12 17:07:02 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, const std::string& password) : port(port), password(password), running(false)
	{
		FD_ZERO(&master_set);
		std::cout << GREEN << "Server constructed for port: " << RESET << port << std::endl;
	}

Server::~Server()
{
	std::map<int, Client*>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
	{
		delete it->second;
		close(it->first);
	}
	std::map<int, CommandHandler*>::iterator itHandler;
	for (itHandler = handlers.begin(); itHandler != handlers.end(); ++itHandler)
	{
		delete itHandler->second;
	}
	 std::cout << RED << "Server destroyed, all clients closed." << RESET << std::endl;
}

bool Server::initialize()
{
	struct sockaddr_in server_addr;
	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener == -1)
	{
		std::cerr << RED << "Error creating socket: " << strerror(errno) << RESET << std::endl;
		return false;
	}

	int opt = 1;
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cerr << RED << "Error setting socket options: " << strerror(errno) << RESET << std::endl;
		close(listener);
		return false;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		std::cerr << RED << "Error binding socket: " << strerror(errno) << RESET << std::endl;
		close(listener);
		return false;
	}

	if (listen(listener, 10) == -1)
	{
		std::cerr << RED << "Error listening: " << strerror(errno) << RESET << std::endl;
		close(listener);
		return false;
	}

	FD_SET(listener, &master_set);
	fdmax = listener; // Set the initial max file descriptor to listener
	running = true;

	std::cout << GREEN << "Server initialized and listening on port: " << RESET << port << std::endl;
	return true;
}

bool Server::getlineFromClient(Client* client, std::string& line)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int len = recv(client->getSocket(), buffer, sizeof(buffer) - 1, 0);

	if (len > 0)
	{
		buffer[len] = '\0'; // Ensure null termination
		line = std::string(buffer);
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
		return true;
	}
	else if (len == 0)
	{
		std::cout << RED << "Client disconnected." << RESET << std::endl;
		return false; // Connection is closed
	}
	else
	{
		std::cerr << RED << "Error reading from client socket: " << strerror(errno) << RESET << std::endl;
		return false; // An error occurred
	}
}

bool Server::processInitialCommands(Client* client)
{
	std::string line;
	bool isAuthenticated = false;  // Pour garder une trace de l'authentification

	while (getlineFromClient(client, line) && !line.empty())
	{
		std::cout << MAGENTA << "Processing line: " << RESET << line << std::endl;
		// Extraction et traitement de la commande PASS
		size_t passEnd = line.find("NICK");
		if (passEnd != std::string::npos)
		{
			std::string passCommand = line.substr(0, passEnd);
			std::istringstream passIss(passCommand);
			std::string passToken, extractedPassword;
			passIss >> passToken >> extractedPassword;
			if (extractedPassword != this->password)
			{
				client->sendMessage("ERROR :Wrong password\r\n");
				std::cout << RED << "Authentication failed for password: " << RESET << extractedPassword << std::endl;
				return false;
			}
			else
			{
				std::cout << "CLIENT Socket: " << client->getSocket() << GREEN << " :Authentication successful." << RESET << std::endl;
				isAuthenticated = true;
			}
		}

		// Traitement des commandes NICK et USER
		if (isAuthenticated)
		{  // Seulement si authentifié
			size_t nickEnd = line.find("USER");
			if (nickEnd != std::string::npos)
			{
				std::string nickCommand = line.substr(passEnd, nickEnd - passEnd);
				std::istringstream nickIss(nickCommand);
				std::string nickToken, nickname;
				nickIss >> nickToken >> nickname;
				client->setNickname(nickname);
				std::cout << BLUE << "NickName SET: " << RESET << nickname << std::endl;
				std::string userCommand = line.substr(nickEnd);
				std::istringstream userIss(userCommand);
				std::string userToken, username, realname;
				userIss >> userToken >> username;
				size_t realnameStart = userCommand.find(":");
				if (realnameStart != std::string::npos)
				{
					realname = userCommand.substr(realnameStart + 1);
					client->setUsername(username);
					client->setRealname(realname);
					std::cout << GREEN << "CLIENT: " << "SET NickName: " << nickname << "; UserName: " << username << "; Socket: " << client->getSocket() << RESET << std::endl;
					break;
				}
			}
		}
	}
	// Après une authentification réussie, passez à la gestion des messages IRC
	if (isAuthenticated)
	{
		handleIRCMessages(client, *new CommandHandler()); // Attention à la gestion de la mémoire ici
	}
	return isAuthenticated;
}
// Handle IRC commands
void Server::handleIRCMessages(Client* client, CommandHandler& cmdHandler)
{
	std::string line;
	while (getlineFromClient(client, line) && !line.empty())
	{
		std::cout << YELLOW << "Handling IRC message: " << RESET << line << std::endl;
		cmdHandler.handleCommand(line, client);
		if (line.find("QUIT") != std::string::npos)
		{
			std::cout << RED << "Client Socket (" << client->getSocket() << ") requested to quit." << RESET << std::endl;
			closeClient(client->getSocket());  // Assuming you have a method to close clients
			break;
		}
	}
}

void Server::run()
{
	fd_set read_fds;
	struct timeval timeout;

	if (!initialize())
	{
		std::cerr << RED << "Server initialization failed. Exiting..." << RESET << std::endl;
		return;
	}

	std::cout << GREEN << "Server is running on port: " << RESET << port << std::endl;
	while (running)
	{
		FD_ZERO(&read_fds);
		FD_SET(listener, &read_fds);
		std::map<int, Client*>::iterator it;
		for (it = clients.begin(); it != clients.end(); ++it)
		{
			FD_SET(it->first, &read_fds);
		}

		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		std::cout << ORANGE << "Waiting for select..." << RESET << std::endl;
		int selectResult = select(fdmax + 1, &read_fds, NULL, NULL, &timeout);

		if (selectResult == -1)
		{
			perror("Select error");
			continue;
		}
		else if (selectResult == 0)
		{
			std::cout << GRAY << "Select timeout occurred, no activity." << RESET << std::endl;
			continue;
		}

		if (FD_ISSET(listener, &read_fds))
		{
			acceptNewClient();
		}

		for (int i = 0; i <= fdmax; i++)
		{
			if (i != listener && FD_ISSET(i, &read_fds))
			{
				if (!handleClientActivity(i))
				{
					closeClient(i);
				}
			}
		}
	}
}

void Server::acceptNewClient()
{
	struct sockaddr_in client_addr;
	socklen_t addrlen = sizeof(client_addr);
	std::cout << CYAN << "Ready to accept new connection..." << RESET << std::endl;
	int newfd = accept(listener, (struct sockaddr *)&client_addr, &addrlen);

	if (newfd == -1)
	{
		perror("Accept failed");
		return;
	}

	clients[newfd] = new Client(newfd);
	FD_SET(newfd, &master_set);
	if (newfd > fdmax)
	{
		fdmax = newfd;
	}

	std::cout << GREEN << "Accepted new client: " << inet_ntoa(client_addr.sin_addr) << " on fd: " << newfd << RESET << std::endl;
}

bool Server::handleClientActivity(int sockfd)
{
	Client* client = clients[sockfd];
	if (!client)
	{
		std::cerr << RED << "Client not found for socket " << RESET << sockfd << std::endl;
		return false;
	}

	std::string line;
	if (getlineFromClient(client, line) && !line.empty())
	{
		std::cout << GREEN << "Received message: [" << line << "]" << RESET << std::endl;
		CommandHandler* cmdHandler = handlers[sockfd];
		cmdHandler->handleCommand(line, client);
		return true;
	}
	else
	{
		std::cerr << RED << "Client disconnected or error on socket: " << RESET << sockfd << std::endl;
		return false;
	}
}

void Server::closeClient(int sockfd)
{
	std::map<int, Client*>::iterator it = clients.find(sockfd);
	if (it != clients.end())
	{
		delete it->second;
		clients.erase(it);
	}

	std::map<int, CommandHandler*>::iterator handlerIt = handlers.find(sockfd);
	if (handlerIt != handlers.end())
	{
		delete handlerIt->second;
		handlers.erase(handlerIt);
	}

	close(sockfd); // Close the socket
	FD_CLR(sockfd, &master_set); // Remove from the master set

	// Recalculate fdmax
	if (sockfd == fdmax)
	{
		fdmax = listener; // Reset fdmax to the listener's socket
		for (it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->first > fdmax)
			{
				fdmax = it->first;
			}
		}
	}

	std::cout << RED << "Closed client on socket: " << RESET << sockfd << std::endl;
}
