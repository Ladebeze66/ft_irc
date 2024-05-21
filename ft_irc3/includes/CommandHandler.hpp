/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 18:14:12 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/21 13:47:12 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Utils.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "AdditionalCommands.hpp"
#include "Who.hpp"
#include "CommandHandler.hpp"
#include "RPL.hpp"

#include <string>
#include <sstream>
#include <algorithm>

class Server;
class ModeWhoHandler;
class AdditinalCommands;

class CommandHandler
{
private:
    Server *_server;

public:
    CommandHandler(Server *server);
	bool isValidNickname(const std::string& nickname);
	bool isNicknameInUse(const std::string& nickname);
	void handleNick(Client* client, const std::vector<std::string>& tokens);

	void handleUser(Client* client, const std::vector<std::string>& tokens);
	void handlePingCommand(Client* client, const std::vector<std::string>& tokens);
    void handleCommand(Client *client, const std::string &command);
	void handleCapCommand(Client* client, const std::vector<std::string>& tokens);
	void handlePassCommand(Client* client, const std::vector<std::string>& tokens);

    void processCommand(Client *client, const std::string &command);
    void handleJoinCommand(Client *client, const std::string &channelName);
    std::string getUsersList(Channel *channel);
};

#endif
