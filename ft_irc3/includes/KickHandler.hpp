/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KickHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 16:59:42 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/06/01 18:51:26 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KICKHANDLER_HPP
#define KICKHANDLER_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <sstream>

class KickHandler
{
	public:
		KickHandler(Server* server);
		void handleKickCommand(Client* client, const std::string& command);

	private:
		Server* _server;
};

#endif