/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InviteHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 13:02:09 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/30 13:08:33 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "InviteHandler.hpp"

InviteHandler::InviteHandler(Server* server) : _server(server) {}

void InviteHandler::handleInviteCommand(Client* client, const std::string& command) {
    std::istringstream iss(command);
    std::string cmd, nickname, channelName;
    iss >> cmd >> nickname >> channelName;

    if (nickname.empty() || channelName.empty()) {
        _server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "INVITE"));
        return;
    }

    Channel* channel = _server->getChannelByName(channelName);
    if (!channel) {
        _server->sendToClient(client->getFd(), ERR_NOSUCHCHANNEL(client->getFd(), channelName));
        return;
    }

    if (!channel->hasClient(client)) {
        _server->sendToClient(client->getFd(), ERR_NOTONCHANNEL(client->getFd(), channelName));
        return;
    }

    if (!channel->isOperator(client) && channel->isInviteOnly()) {
        _server->sendToClient(client->getFd(), ERR_CHANOPRIVSNEEDED(client->getFd(), channelName));
        return;
    }

    Client* targetClient = _server->getClientByName(nickname);
    if (!targetClient) {
        _server->sendToClient(client->getFd(), ERR_NOSUCHNICK(client->getFd(), nickname));
        return;
    }

    if (channel->hasClient(targetClient)) {
        _server->sendToClient(client->getFd(), ERR_USERONCHANNEL(client, nickname, channelName));
        return;
    }

    channel->addInvitedClient(targetClient);
    _server->sendToClient(client->getFd(), RPL_INVITING(client, nickname, channelName));

    std::ostringstream inviteMsg;
    inviteMsg << ":" << client->getNickname() << " INVITE " << nickname << " " << channelName << "\r\n";
    _server->sendToClient(targetClient->getFd(), inviteMsg.str());
}