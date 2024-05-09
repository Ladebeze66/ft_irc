/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:19:30 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/09 14:49:16 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include "Utilities.hpp"
#include <map>
#include <string>
#include <sstream>
#include <iterator>

class CommandHandler {
public:
    CommandHandler();
    ~CommandHandler();
    void handleCommand(const std::string& command, Client* client);

private:
    std::map<std::string, Channel*> channels;

    void handleJoin(const std::string& command, Client* client);
    void handlePart(const std::string& command, Client* client);
    void handleNick(const std::string& command, Client* client);
    void handlePrivmsg(const std::string& command, Client* client);
    void handleQuit(Client* client);

    std::vector<std::string> split(const std::string& s, char delimiter);
};

#endif // COMMANDHANDLER_HPP
