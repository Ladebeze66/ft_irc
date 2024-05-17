/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeWhoHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 16:08:48 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/17 18:08:06 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODEWHOHANDLER_HPP
#define MODEWHOHANDLER_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Utils.hpp"

class Server;

class ModeWhoHandler
{
public:
    ModeWhoHandler(Server *server);
    void handleModeCommand(Client *client, const std::string &command);
    void handleWhoCommand(Client *client, const std::string &command);

private:
    Server *_server;
};

#endif
