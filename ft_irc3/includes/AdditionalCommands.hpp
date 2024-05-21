/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AdditionalCommands.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 18:09:05 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/21 19:46:40 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ADDITIONALCOMMANDS_HPP
#define ADDITIONALCOMMANDS_HPP

#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "CommandHandler.hpp"
#include "Utils.hpp"
#include "RPL.hpp"
#include "Who.hpp"

#include <sstream>
#include <string>
#include <fstream>

// Forward declarations
class Server;
class Client;
class Channel;
class CommandHandler;

class AdditionalCommands
{
private:
    Server *_server;
	CommandHandler *_commandhandler;
public:
    AdditionalCommands(Server *server);
    void processCommand(Client *client, const std::string &command);
    void broadcastChannelList(Client *client, Server *server);
    void handlePartCommand(Server *server, Client *client, const std::string &command);
    void handleNickCommand(Server *server, Client *client, const std::string &command);
    void handlePrivmsgCommand(Server *server, Client *client, const std::string &command);
};

#endif // ADDITIONALCOMMANDS_HPP
