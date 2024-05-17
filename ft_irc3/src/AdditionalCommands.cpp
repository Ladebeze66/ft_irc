/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AdditionalCommands.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:27:29 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/17 20:15:14 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AdditionalCommands.hpp"

// Fonction pour gérer la commande PART
void handlePartCommand(Server *server, Client *client, const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd, channelName;
    iss >> cmd >> channelName;

    std::map<std::string, Channel *> &channels = server->getChannels();

    if (channels.find(channelName) != channels.end())
    {
        Channel *channel = channels[channelName];
        channel->removeClient(client);

        std::stringstream partMsg;
        partMsg << ":" << client->getNickname() << " PART " << channelName << "\r\n";
        server->sendToClient(client->getFd(), partMsg.str());

        if (channel->isEmpty())
        {
            delete channel;
            channels.erase(channelName);
        }

        server->log("Client " + client->getNickname() + " left channel " + channelName, MAGENTA);
    }
    else
    {
        std::stringstream ss;
        ss << ":server 403 " << client->getNickname() << " " << channelName << " :No such channel\r\n";
        server->sendToClient(client->getFd(), ss.str());
    }
}

// Fonction pour gérer la commande NICK
void handleNickCommand(Server *server, Client *client, const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd, newNick;
    iss >> cmd >> newNick;

    std::stringstream nickMsg;
    nickMsg << ":" << client->getNickname() << " NICK " << newNick << "\r\n";
    server->sendToClient(client->getFd(), nickMsg.str());

    client->setNickname(newNick);

    std::stringstream ss;
    ss << "Client " << client->getFd() << " changed nickname to " << newNick;
    server->log(ss.str(), GREEN);
}

void handlePrivmsgCommand(Server *server, Client *client, const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd, target, message;
    iss >> cmd >> target;
    getline(iss, message);

    // Enlever le ':' initial dans le message si présent
    if (!message.empty() && message[0] == ':')
        message = message.substr(1);

    // Récupérer les canaux du serveur
    std::map<std::string, Channel *> &channels = server->getChannels();

    // Si la cible est un canal
    if (channels.find(target) != channels.end())
    {
        Channel *channel = channels[target];
        std::vector<Client *> channelClients = channel->getClients();

        for (size_t i = 0; i < channelClients.size(); ++i)
        {
            // Envoyer le message à tous les clients du canal sauf l'émetteur
            if (channelClients[i] != client)
            {
                std::stringstream privMsg;
                privMsg << ":" << client->getNickname() << " PRIVMSG " << target << " :" << message << "\r\n";
                server->sendToClient(channelClients[i]->getFd(), privMsg.str());
            }
        }
    }
    // Si la cible est un utilisateur
    else
    {
        Client *targetClient = server->getClientByName(target); // Utiliser getClientByName pour trouver le client par nom

        if (targetClient)
        {
            std::stringstream privMsg;
            privMsg << ":" << client->getNickname() << " PRIVMSG " << target << " :" << message << "\r\n";
            server->sendToClient(targetClient->getFd(), privMsg.str());
        }
        else
        {
            // Si la cible n'est ni un canal ni un utilisateur existant, envoyer un message d'erreur
            std::stringstream errorMsg;
            errorMsg << ":server 401 " << client->getNickname() << " " << target << " :No such nick/channel\r\n";
            server->sendToClient(client->getFd(), errorMsg.str());
        }
    }
}
