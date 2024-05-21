/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Welcome.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:53:52 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/21 19:54:48 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Welcome.hpp"

void WelcomeHandler::sendWelcomeMessages(Client *client, Server *server)
{
	server->sendToClient(client->getFd(), RPL_WELCOME(client));
	server->sendToClient(client->getFd(), RPL_YOURHOST(client));
	server->sendToClient(client->getFd(), RPL_CREATED(client));
	server->sendToClient(client->getFd(), RPL_MYINFO(client));
	server->sendToClient(client->getFd(), RPL_ISUPPORT(client, "MODES=EXAMPLE"));

	sendMotd(client, server);
}

void WelcomeHandler::sendMotd(Client *client, Server *server)
{
	std::ifstream motdFile("motd.txt");
	if (motdFile.is_open())
	{
		std::string line;
		server->sendToClient(client->getFd(), RPL_MOTDSTART(client));
		while (std::getline(motdFile, line))
		{
			server->sendToClient(client->getFd(), RPL_MOTD(client, line));
		}
		server->sendToClient(client->getFd(), RPL_ENDOFMOTD(client));
		motdFile.close();
	}
	else
	{
		server->sendToClient(client->getFd(), ERR_NOMOTD(client));
	}
}
