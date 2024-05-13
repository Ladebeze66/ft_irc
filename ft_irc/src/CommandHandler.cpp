/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:19:15 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/12 18:07:48 by fgras-ca         ###   ########.fr       */
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

void CommandHandler::handleCommand(const std::string& commandLine, Client* client) {
    std::istringstream stream(commandLine);
    std::string command;
    stream >> command;

    if (command == "JOIN") {
        handleJoin(commandLine, client);
    } else if (command == "PART") {
        handlePart(commandLine, client);
    } else if (command == "NICK") {
        handleNick(commandLine, client);
    } else if (command == "PRIVMSG") {
        handlePrivmsg(commandLine, client);
    }
}

void CommandHandler::handleJoin(const std::string& command, Client* client) {
    std::cout << "Entering handleJoin with command: " << command << std::endl;
    std::vector<std::string> tokens = split(command, ' ');
    if (tokens.size() > 1) {
        std::string channelName = tokens[1];

        // Log before accessing the map
        std::cout << "Checking if channel exists: " << channelName << std::endl;

        // Check if channel exists
        std::map<std::string, Channel*>::iterator it = channels.find(channelName);
        if (it == channels.end()) {
            // If not, create a new channel
            std::cout << "Creating new channel: " << channelName << std::endl;
            channels[channelName] = new Channel(channelName);
        } else {
            std::cout << "Channel already exists: " << channelName << std::endl;
        }

        // Add client to channel and send join message
        channels[channelName]->addClient(client);
        channels[channelName]->broadcastMessage("JOIN " + channelName, client);
    } else {
        std::cerr << "Invalid JOIN command format." << std::endl;
    }
}



void CommandHandler::handlePart(const std::string& command, Client* client) {
    std::vector<std::string> tokens = split(command, ' ');
    if (tokens.size() > 1) {
        std::string channelName = tokens[1];
        if (channels.find(channelName) != channels.end()) {
            channels[channelName]->removeClient(client);
        }
    }
}

void CommandHandler::handleNick(const std::string& command, Client* client) {
    std::vector<std::string> tokens = split(command, ' ');
    if (tokens.size() > 1) {
        client->setNickname(tokens[1]);
    }
}

void CommandHandler::handlePrivmsg(const std::string& command, Client* client) {
    std::vector<std::string> tokens = split(command, ' ');
    if (tokens.size() > 2) {
        std::string target = tokens[1];
        std::string message = command.substr(command.find(':') + 1);
        if (channels.find(target) != channels.end()) {
            channels[target]->broadcastMessage("PRIVMSG " + target + " :" + message, client);
        }
    }
}

std::vector<std::string> CommandHandler::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
