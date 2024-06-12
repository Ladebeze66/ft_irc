/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InviteHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 13:01:50 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/06/12 11:05:09 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INVITEHANDLER_HPP
#define INVITEHANDLER_HPP

#include <string>
#include <vector>
#include <sstream>

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "RPL.hpp"
#include "Utils.hpp"

class Server;
class Client;
class Channel;

class InviteHandler
{
	public:
		InviteHandler(Server* server);
		~InviteHandler();
		void handleInviteCommand(Client* client, const std::string& command);

	private:
		Server* _server;
};

#endif
