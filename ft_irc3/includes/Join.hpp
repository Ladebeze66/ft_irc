/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 19:51:08 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/21 20:13:15 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JOIN_HPP
#define JOIN_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "RPL.hpp"

class Server;
class Client;
class Channel;

class JoinHandler
{
public:
    void handleJoinCommand(Client *client, const std::string &channelName, Server *server);

private:
    std::string getUsersList(Channel *channel);
};

#endif // JOIN_HPP
