/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:13:08 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/30 13:33:36 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ModeHandler.hpp"

ModeHandler::ModeHandler(Server* server) : _server(server) {}

void ModeHandler::handleModeCommand(Client* client, const std::string& command) {
	_server->log("Received MODE command: " + command, "\033[1;34m");
    std::vector<std::string> tokens = split(command, " \n\r\t");

    if (tokens.size() < 2) {
        _server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "MODE"));
		_server->log("MODE command error: Need more parameters", "\033[1;31m");
        return;
    }

    std::string target = tokens[1];
    if (target[0] == '#' || target[0] == '&') {
		_server->log("Handling channel mode for target: " + target, "\033[1;34m");
        handleChannelMode(client, tokens);
    } else {
		_server->log("Handling user mode for target: " + target, "\033[1;34m");
        handleUserMode(client, tokens);
    }
}

void ModeHandler::handleUserMode(Client* client, const std::vector<std::string>& tokens) {
    std::string target = tokens[1];
    if (target != client->getNickname()) {
        _server->sendToClient(client->getFd(), ERR_USERSDONTMATCH(client->getFd()));
		_server->log("User mode error: Users don't match", "\033[1;31m");
        return;
    }

    if (tokens.size() == 2) {
        _server->sendToClient(client->getFd(), RPL_UMODEIS(client->getFd(), "")); // Ajout de la chaîne vide pour les modes utilisateur
		 _server->log("Sent user modes to client", "\033[1;32m");
    } else {
        std::string modeString = tokens[2];
        // Gestion des modes utilisateur (ajout ou suppression)
        // Par exemple, vous pouvez gérer l'ajout ou la suppression du mode invisible ici
		 _server->log("Updating user modes: " + modeString, "\033[1;34m");
    }
}

void ModeHandler::handleChannelMode(Client* client, const std::vector<std::string>& tokens) {
    std::string channelName = tokens[1];
    Channel* channel = _server->getChannelByName(channelName);
    if (!channel) {
        _server->sendToClient(client->getFd(), ERR_NOSUCHCHANNEL(client->getFd(), channelName));
		_server->log("Channel mode error: No such channel " + channelName, "\033[1;31m");
        return;
    }

    if (tokens.size() == 2) {
        // Retourner les modes actuels du canal
        std::ostringstream oss;
        oss << RPL_CHANNELMODEIS(client->getFd(), channel->getName(), channel->getModes());
        oss << RPL_CREATIONTIME(client->getFd(), channel->getName(), channel->getTopicTime());
        _server->sendToClient(client->getFd(), oss.str());
		 _server->log("Sent channel modes to client", "\033[1;32m");
        return;
    }

    if (!channel->isOperator(client)) {
        _server->sendToClient(client->getFd(), ERR_CHANOPRIVSNEEDED(client->getFd(), channelName));
		_server->log("Channel mode error: Channel operator privileges needed for " + channelName, "\033[1;31m");
        return;
    }

    std::string modeString = tokens[2];
    bool adding = true;
    size_t argIndex = 3;

    for (size_t i = 0; i < modeString.length(); ++i) {
        char mode = modeString[i];
        if (mode == '+') {
            adding = true;
        } else if (mode == '-') {
            adding = false;
        } else if (mode == 'b' || mode == 'l' || mode == 'i' || mode == 'k' || mode == 't') {
            std::string argument;
            if (argIndex < tokens.size()) {
                argument = tokens[argIndex++];
            }
            setChannelMode(channel, std::string(1, mode), adding, argument);
        } else {
            _server->sendToClient(client->getFd(), ERR_UNKNOWNMODE(client, mode, channelName));
        }
    }
}

void ModeHandler::setChannelMode(Channel* channel, const std::string& mode, bool adding, const std::string& argument) {
    /*if (mode == "b") {
        applyModeB(channel, adding, argument);
    }*/
	if (mode == "l") {
        applyModeL(channel, adding, argument);
    } else if (mode == "i") {
        applyModeI(channel, adding);
    } else if (mode == "k") {
        applyModeK(channel, adding, argument);
    } else if (mode == "t") {
        applyModeT(channel, adding);
    }
}

/*void ModeHandler::applyModeB(Channel* channel, bool adding, const std::string& argument) {
    // Implémentation de l'ajout ou de la suppression de bannissements
    // Note : Vous pouvez ajouter des méthodes à la classe Channel pour gérer la liste des bannissements
    if (adding) {
        // Ajouter le masque à la liste des bannissements
        // channel->banClient(argument);
    } else {
        // Supprimer le masque de la liste des bannissements
        // channel->unbanClient(argument);
    }
}*/

void ModeHandler::applyModeL(Channel* channel, bool adding, const std::string& argument) {
    if (adding) {
        int limit = std::atoi(argument.c_str());
        channel->setClientLimit(limit);
    } else {
        channel->setClientLimit(0); // Retirer la limite
    }
}

void ModeHandler::applyModeI(Channel* channel, bool adding) {
    channel->setInviteOnly(adding);
}

void ModeHandler::applyModeK(Channel* channel, bool adding, const std::string& argument) {
    if (adding) {
        channel->setKey(argument);
    } else {
        channel->setKey("");
    }
}

void ModeHandler::applyModeT(Channel* channel, bool adding) {
    channel->setTopicProtection(adding);
}

