/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:01:34 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/13 19:19:46 by fgras-ca         ###   ########.fr       */
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

bool Server::getlineFromClient(Client* client, std::string& line) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(client->getSocket(), buffer, sizeof(buffer) - 1, 0);

        if (len > 0) {
            buffer[len] = '\0';
            line = std::string(buffer);
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
            std::cout << GREEN << "Received line: " << line << RESET << std::endl;
            return true;
        } else if (len == 0) {
            std::cout << RED << "Client disconnected." << RESET << std::endl;
            return false;
        } else {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // Si non-bloquant et aucune donnée n'est disponible, attendez un peu et réessayez
                std::cout << YELLOW << "No data available yet, non-blocking mode." << RESET << std::endl;
                usleep(100000); // attendre 100 ms
                continue; // Continuez à essayer de lire
            } else {
                std::cerr << RED << "Error reading from client socket: " << strerror(errno) << RESET << std::endl;
                return false;
            }
        }
    }
}




bool Server::processInitialCommands(Client* client) {
    std::string line;
    bool isAuthenticated = false;

    if (!getlineFromClient(client, line) || line.empty()) {
        std::cout << RED << "No initial commands received or connection lost." << RESET << std::endl;
        return false;
    }

    std::cout << MAGENTA << "Processing initial line: " << RESET << line << std::endl;

    // Extrait les parties nécessaires de la ligne
    size_t passIndex = line.find("PASS");
    size_t nickIndex = line.find("NICK");
    size_t userIndex = line.find("USER");

    if (passIndex == std::string::npos || nickIndex == std::string::npos || userIndex == std::string::npos) {
        std::cerr << "Command format error." << std::endl;
        return false;
    }

    // Extraction des données
    std::string password = line.substr(passIndex + 4, nickIndex - (passIndex + 4));
    std::string nickname = line.substr(nickIndex + 4, userIndex - (nickIndex + 4));
    std::string userAndRealName = line.substr(userIndex + 4);  // Contient le username et le realname
    size_t spaceIndex = userAndRealName.find(' ');
    std::string username = userAndRealName.substr(0, spaceIndex);
    std::string realname = userAndRealName.substr(spaceIndex + 1);

    // Retire les espaces potentiels autour des informations
    password.erase(remove(password.begin(), password.end(), ' '), password.end());
    nickname.erase(remove(nickname.begin(), nickname.end(), ' '), nickname.end());
    username.erase(remove(username.begin(), username.end(), ' '), username.end());

    // Validation du mot de passe
    if (password != this->password) {
        client->sendMessage("ERROR :Wrong password\r\n");
        std::cout << RED << "Authentication failed for password: " << password << RESET << std::endl;
        return false;
    }

    // Set up client details if password is correct
    client->setNickname(nickname);
    client->setUsername(username);
    client->setRealname(realname);
    isAuthenticated = true;

    std::cout << GREEN << "Authentication successful, client setup complete." << RESET << std::endl;
    std::cout << GREEN << "Client details - Nickname: " << nickname << ", Username: " << username << ", Realname: " << realname << RESET << std::endl;

    // Proceed to handle IRC messages if authenticated
    if (isAuthenticated) {
        handleIRCMessages(client, *new CommandHandler());
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

void Server::acceptNewClient() {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    std::cout << CYAN << "Prêt à accepter une nouvelle connexion..." << RESET << std::endl;
    int newfd = accept(listener, (struct sockaddr *)&client_addr, &addrlen);

    if (newfd == -1) {
        perror("Échec de l'acceptation");
        return;
    }

    int flags = fcntl(newfd, F_GETFL, 0);
    if (flags == -1) flags = 0;
    fcntl(newfd, F_SETFL, flags | O_NONBLOCK);

    Client* newClient = new Client(newfd);
    if (!newClient) {
        std::cerr << RED << "Échec de la création d'une nouvelle instance Client." << RESET << std::endl;
        close(newfd);
        return;
    }

    CommandHandler* newHandler = new CommandHandler();
    if (!newHandler) {
        std::cerr << RED << "Échec de la création d'une nouvelle instance de CommandHandler." << RESET << std::endl;
        delete newClient;
        close(newfd);
        return;
    }

    clients[newfd] = newClient;
    handlers[newfd] = newHandler;
    FD_SET(newfd, &master_set);
    if (newfd > fdmax) {
        fdmax = newfd;
    }

    std::cout << GREEN << "Nouveau client accepté: " << inet_ntoa(client_addr.sin_addr) << " sur fd: " << newfd << RESET << std::endl;

    if (!processInitialCommands(newClient)) {
        std::cerr << "Échec du traitement des commandes initiales, fermeture de la connexion." << std::endl;
        closeClient(newfd);
    }
}




bool Server::handleClientActivity(int sockfd) {
    Client* client = clients[sockfd];
    CommandHandler* cmdHandler = handlers[sockfd];  // Assurez-vous que cmdHandler est récupéré correctement

    if (!client || !cmdHandler) {
        std::cerr << RED << "Client or CommandHandler not found for socket " << RESET << sockfd << std::endl;
        return false;
    }

    std::string line;
    if (getlineFromClient(client, line) && !line.empty()) {
        std::cout << GREEN << "Received message: [" << line << "]" << RESET << std::endl;
        cmdHandler->handleCommand(line, client);
        return true;
    } else {
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
