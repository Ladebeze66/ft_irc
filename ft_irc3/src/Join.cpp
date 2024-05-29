/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 19:51:31 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/29 13:24:35 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Join.hpp"

void JoinHandler::handleJoinCommand(Client *client, const std::string &channelName, Server *server)
{
    std::map<std::string, Channel *> &channels = server->getChannels();
    Channel *channel;

	std::vector<std::string>	lst = split(channelName, ",");
	for (std::vector<std::string>::iterator itl = lst.begin(); itl != lst.end(); ++itl)
	{
		std::cout << "hjoincmd .. " << (*itl) << std::endl; 
		// ERR_TOOMANYCHANNELS (405)
		// ERR_NEEDMOREPARAMS (461)//?
		// ERR_CHANNELISFULL (471)
		// ERR_INVITEONLYCHAN (473)
		// ERR_BADCHANNELKEY (475)
		// RPL_TOPIC (332)
		// RPL_TOPICWHOTIME (333)

		if((*itl)[0] == '0' && (*itl).size() == 1)
		{
			//leave all
			continue;
		}
		// check le nom du chanel
		else if ((*itl)[0] != '#' && (*itl)[0] != '&')
		{
			// ERR_NOSUCHCHANNEL (403)
			// ERR_BADCHANMASK (476)
			server->sendToClient(client->getFd(), "bad name \r\n");
			continue;
		}

		if (channels.find((*itl)) == channels.end())
		{
			// if (channels[*itl]->isBanned(client))
			// {
			// 	// ERR_BANNEDFROMCHAN (474)
			// 	server->sendToClient(client->getFd(), "ban from channel \r\n");
			// 	continue;
			// }
			channel = new Channel((*itl));
			channels[(*itl)] = channel;
			channel->addOperator(client);//set op
		}
		else
		{
			channel = channels[(*itl)];
		}

		channel->addClient(client);

		std::vector<Client *> clients = channel->getClients();
		for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			std::string cmd = ":" + client->getNickname() + " JOIN " + channel->getName() + "\r\n";
			server->sendToClient((*it)->getFd(), cmd);
		}

		std::string usersList = getUsersList(channel);
		server->sendToClient(client->getFd(), usersList);
		}
}

std::string JoinHandler::getUsersList(Channel *channel)
{
    std::vector<Client *> clients = channel->getClients();
    std::string usersList = ":server 353 " + clients[0]->getNickname() + " = " + channel->getName() + " :";
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
		if (channel->isOperator((*it)))
       		usersList += "@";
        usersList += (*it)->getNickname() + " ";
    }
    usersList += "\r\n:server 366 " + clients[0]->getNickname() + " " + channel->getName() + " :End of /NAMES list.\r\n";
    return usersList;
}