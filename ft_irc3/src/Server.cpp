/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:17:12 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/16 18:52:19 by fgras-ca         ###   ########.fr       */
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

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(_port);

    if (bind(_server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        log("Failed to bind socket.", RED);
        exit(EXIT_FAILURE);
    }

    if (listen(_server_fd, SOMAXCONN) == -1)
    {
        log("Failed to listen on socket.", RED);
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
                else
                {
                    _clientManager->handleClient(_poll_fds[i].fd);
                }
            }
        }

        handleServerCommands();
    }
}

void Server::handleServerCommands()
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    struct timeval tv = {0, 0};  // non-blocking

    int activity = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

    if (activity > 0 && FD_ISSET(STDIN_FILENO, &readfds))
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
}

void Server::log(const std::string &message, const std::string &color)
{
    std::cout << color << message << RESET << std::endl;
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


std::map<int, Client *> &Server::getClients()
{
    return _clients;
}

std::map<std::string, Channel *> &Server::getChannels()
{
    return _channels;
}

const std::string &Server::getPassword() const
{
    return _password;
}


/* Explications des Fonctions
Server::Server(int port, const std::string &password)

Description: Constructeur de la classe Server. Initialise le serveur avec le port et le mot de passe fournis.
Paramètres:
int port: Le port sur lequel le serveur écoute.
const std::string &password: Le mot de passe requis pour se connecter au serveur.
Détails: Appelle initServer() pour initialiser les paramètres du socket et commencer l'écoute des connexions.
Server::~Server()

Description: Destructeur de la classe Server. Ferme le socket du serveur et libère la mémoire des clients.
Détails: Parcourt la map _clients et delete chaque objet Client.
void Server::initServer()

Description: Initialise le socket du serveur et configure le binding et l'écoute.
Détails: Crée un socket avec socket(), lie le socket à une adresse avec bind(), et met le socket en mode écoute avec listen().
void Server::run()

Description: Boucle principale du serveur qui gère les connexions entrantes et les communications avec les clients.
Détails: Utilise poll() pour surveiller les événements sur les sockets. Accepte les nouvelles connexions avec acceptClient() et gère les communications avec handleClient().
void Server::acceptClient()

Description: Accepte une nouvelle connexion client et l'ajoute à la liste des clients.
Détails: Crée un nouvel objet Client et l'ajoute à la map _clients. Ajoute le descripteur de fichier du client à _poll_fds.
void Server::handleClient(int client_fd)

Description: Gère les messages entrants des clients.
Paramètres:
int client_fd: Le descripteur de fichier du client.
Détails: Reçoit les messages du client et vérifie l'authentification. Si le client n'est pas authentifié, vérifie le mot de passe. Si le client est authentifié, répond au message.
void Server::removeClient(int client_fd)

Description: Supprime un client de la liste et ferme la connexion.
Paramètres:
int client_fd: Le descripteur de fichier du client.
Détails: Ferme le socket du client et le supprime de la map _clients et du vecteur _poll_fds.
void Server::log(const std::string &message, const std::string &color)

Description: Enregistre un message dans les logs et l'affiche avec des couleurs dans la console.
Paramètres:
const std::string &message: Le message à enregistrer.
const std::string &color: La couleur du message.
Détails: Utilise logMessage() pour enregistrer et afficher le message.
void Server::sendToClient(int client_fd, const std::string &message)

Description: Envoie un message à un client.
Paramètres:
int client_fd: Le descripteur de fichier du client.
const std::string &message: Le message à envoyer.
Détails: Utilise send() pour envoyer le message au client.

Lecture Ligne par Ligne : Utilisez std::istringstream pour lire le message ligne par ligne et traiter chaque commande séparément.
Suppression des Caractères de Fin de Ligne : Utilisez erase et std::remove pour supprimer les caractères \r et \n de chaque ligne avant de traiter la commande.
Vérification du Mot de Passe : Si le client n'est pas authentifié, vérifiez s'il envoie la commande PASS et validez le mot de passe. Sinon, répondez avec un message demandant le mot de passe.
Explications des Modifications
Notification de Jonction : Lorsque le client rejoint un canal, le serveur envoie une notification de jonction avec la commande JOIN.


std::stringstream joinMsg;
joinMsg << ":" << client->getFd() << " JOIN " << channelName << "\r\n";
sendToClient(client->getFd(), joinMsg.str());
Liste des Utilisateurs : Le serveur envoie la liste des utilisateurs dans le canal en utilisant le code de réponse 353.


std::string usersList = getUsersList(channel);
sendToClient(client->getFd(), usersList);
Fin de la Liste des Utilisateurs : Le serveur envoie un message de fin de liste des utilisateurs avec le code de réponse 366.

std::stringstream endOfNamesMsg;
endOfNamesMsg << ":server 366 " << client->getFd() << " " << channelName << " :End of /NAMES list.\r\n";
sendToClient(client->getFd(), endOfNamesMsg.str());
Fonction getUsersList : Cette fonction génère la liste des utilisateurs dans le canal en utilisant le format correct.

std::string Server::getUsersList(Channel *channel)
{
    std::vector<Client *> clients = channel->getClients();
    std::stringstream ss;
    ss << ":server 353 " << clients[0]->getFd() << " = " << channel->getName() << " :";
    for (size_t i = 0; i < clients.size(); ++i)
    {
 	   if (i > 0)
 		   ss << " ";
 	   ss << clients[i]->getFd();
    }
    ss << "\r\n";
    return ss.str();
}
Avec ces modifications, votre serveur IRC devrait maintenant informer correctement le client HexChat des utilisateurs présents dans un canal, ce qui devrait faire apparaître les canaux et leurs utilisateurs correctement dans l'interface du client.

*/