/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 19:51:08 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/30 17:48:55 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JOIN_HPP
#define JOIN_HPP

#include <string>
#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "RPL.hpp"
#include "Utils.hpp"

class JoinHandler
{
public:
	void handleJoinCommand(Client *client, const std::string &channelName, Server *server);

private:
	void sendJoinSuccess(Client *client, Channel *channel, Server *server);
	std::string getUsersList(Channel *channel);
};

#endif // JOIN_HPP
