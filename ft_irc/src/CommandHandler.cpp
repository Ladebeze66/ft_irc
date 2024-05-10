/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:19:15 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/10 12:33:00 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

CommandHandler::CommandHandler() {}

CommandHandler::~CommandHandler() {
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        delete it->second;
    }
    channels.clear();
}

void CommandHandler::handleCommand(const std::string& command, Client* client) {
    if (command.find("JOIN ") == 0) {
        handleJoin(command, client);
    } else if (command.find("PART ") == 0) {
        handlePart(command, client);
    } else if (command.find("NICK ") == 0) {
        handleNick(command, client);
    } else if (command.find("PRIVMSG ") == 0) {
        handlePrivmsg(command, client);
    } else if (command == "QUIT") {
        handleQuit(client);
    }
}

void CommandHandler::handleJoin(const std::string& command, Client* client) {
    std::vector<std::string> tokens = split(command, ' ');
    if (tokens.size() > 1) {
        std::string channelName = tokens[1];
        if (channels.find(channelName) == channels.end()) {
            channels[channelName] = new Channel(channelName);
        }
        channels[channelName]->addClient(client);
        channels[channelName]->broadcastMessage("JOIN " + channelName, client);
    }
}

void CommandHandler::handlePart(const std::string& command, Client* client) {
    std::vector<std::string> tokens = split(command, ' ');
    if (tokens.size() > 1) {
        std::string channelName = tokens[1];
        if (channels.find(channelName) != channels.end()) {
            channels[channelName]->removeClient(client);
            channels[channelName]->broadcastMessage("PART " + channelName, client);
        }
    }
}

void CommandHandler::handleNick(const std::string& command, Client* client) {
    std::vector<std::string> tokens = split(command, ' ');
    if (tokens.size() > 1) {
        client->setNickname(tokens[1]);
        std::cout << "Nickname set to: " << tokens[1] << std::endl; // Afficher et enregistrer l'action
    }
}


void CommandHandler::handlePrivmsg(const std::string& command, Client* client) {
    std::vector<std::string> tokens = split(command, ' ');
    if (tokens.size() > 2) {
        std::string target = tokens[1];
        std::string message = command.substr(command.find(" :") + 2);
        if (channels.find(target) != channels.end()) {
            channels[target]->broadcastMessage("PRIVMSG " + message, client);
        }
    }
}

void CommandHandler::handleQuit(Client* client) {
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        it->second->removeClient(client);
    }
}

std::vector<std::string> CommandHandler::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delimiter)) {
        tokens.push_back(item);
    }
    return tokens;
}
