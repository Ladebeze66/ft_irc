/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 18:14:12 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/16 17:35:46 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Utils.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "AdditionalCommands.hpp"

#include <string>
#include <sstream>
#include <algorithm>

class Server;

class CommandHandler
{
private:
    Server *_server;

public:
    CommandHandler(Server *server);
    void handleCommand(Client *client, const std::string &command);
    void processCommand(Client *client, const std::string &command);
    void handleJoinCommand(Client *client, const std::string &channelName);
    std::string getUsersList(Channel *channel);
};

#endif
