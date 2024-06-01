/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:12:57 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/06/01 18:52:10 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODEHANDLER_HPP
#define MODEHANDLER_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "RPL.hpp"
#include "Utils.hpp"

class Server;
class Client;
class Channel;

class ModeHandler
{
	public:
		ModeHandler(Server* server);
		void handleModeCommand(Client* client, const std::string& command);

	private:
		Server* _server;

		void handleUserMode(Client* client, const std::vector<std::string>& tokens);
		void handleChannelMode(Client* client, const std::vector<std::string>& tokens);

		void setChannelMode(Client *client, Channel* channel, const std::string& mode, bool adding, const std::string& argument);
		void applyModeL(Client *client, Channel* channel, bool adding, const std::string& argument);
		void applyModeI(Channel* channel, bool adding);
		void applyModeK(Client *client, Channel* channel, bool adding, const std::string& argument);
		void applyModeT(Channel* channel, bool adding);
		void applyModeO(Client *client, Channel* channel, bool adding, const std::string& argument);
};

#endif
