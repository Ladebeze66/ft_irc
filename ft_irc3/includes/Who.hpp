/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Who.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 16:08:48 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/06/01 19:03:06 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WHO_HPP
#define WHO_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Utils.hpp"
#include "RPL.hpp"

#include <sstream>
#include <iostream>

class Server;
class WhoHandler
{
	public:
		WhoHandler(Server *server);
		void handleWhoCommand(Client *client, const std::string &command);
		void handleWhoisCommand(Client *client, const std::string &command);

	private:
		Server *_server;

};

#endif
