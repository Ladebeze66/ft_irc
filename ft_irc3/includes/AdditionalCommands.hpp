/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AdditionalCommands.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:23:58 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/19 19:18:11 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ADDITIONALCOMMANDS_HPP
#define ADDITIONALCOMMANDS_HPP

#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Utils.hpp"
#include "RPL.hpp"
#include <sstream>
#include <string>
#include <fstream>

// Forward declarations
class Server;
class Client;
class Channel;

void sendWelcomeMessages(Client *client, Server *server);
void sendMotd(Client *client, Server *server);
void broadcastChannelList(Client *client, Server *server);
void handlePartCommand(Server *server, Client *client, const std::string &command);
void handleNickCommand(Server *server, Client *client, const std::string &command);
void handlePrivmsgCommand(Server *server, Client *client, const std::string &command);

#endif // ADDITIONALCOMMANDS_HPP
