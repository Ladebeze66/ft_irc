/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPL.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 15:12:47 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/19 19:07:25 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPL_HPP
#define RPL_HPP

#include <string>
#include <sstream>
#include "Client.hpp"

#define SERVER_NAME "IRC_Server"
#define SERVER_VERSION "1.0"
#define USER_MODES "iw"
#define CHANNEL_MODES "nt"
#define CHANNEL_MODES_WITH_PARAMS "kl"



// Macros pour accéder aux champs du client
#define CLIENT_FD(client) (client->getFd())
#define CLIENT_NICK(client) ((client)->getNickname())
#define CLIENT_USER(client) ((client)->getUser())
#define CLIENT_HOST(client) ((client)->getHost())

// Fonctions pour générer les réponses RPL
inline std::string RPL_WELCOME(Client* client) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 001 " << CLIENT_FD(client)
        << " :Welcome to the Internet Relay Network " << CLIENT_NICK(client)
        << "!" << CLIENT_USER(client) << "@" << CLIENT_HOST(client) << "\r\n";
    return oss.str();
}

inline std::string RPL_YOURHOST(Client* client) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 002 " << CLIENT_FD(client)
        << " :Your host is " << SERVER_NAME << ", running version " << SERVER_VERSION << "\r\n";
    return oss.str();
}

inline std::string RPL_CREATED(Client* client) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 003 " << CLIENT_FD(client)
        << " :This server was created " << __DATE__ << "\r\n";
    return oss.str();
}

inline std::string RPL_MYINFO(Client* client) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 004 " << CLIENT_FD(client) << " "
        << SERVER_NAME << " " << SERVER_VERSION << " "
        << USER_MODES << " " << CHANNEL_MODES << " "
        << CHANNEL_MODES_WITH_PARAMS << "\r\n";
    return oss.str();
}

inline std::string RPL_ISUPPORT(Client* client, const std::string& tokens) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 005 " << CLIENT_FD(client)
        << " " << tokens << " :are supported by this server\r\n";
    return oss.str();
}

inline std::string RPL_MOTDSTART(Client* client) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 375 " << CLIENT_FD(client)
        << " :- " << SERVER_NAME << " Message of the day - \r\n";
    return oss.str();
}

inline std::string RPL_MOTD(Client* client, const std::string& line) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 372 " << CLIENT_FD(client)
        << " :- " << line << "\r\n";
    return oss.str();
}

inline std::string RPL_ENDOFMOTD(Client* client) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 376 " << CLIENT_FD(client)
        << " :End of /MOTD command.\r\n";
    return oss.str();
}

inline std::string ERR_NOMOTD(Client* client) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 422 " << CLIENT_FD(client)
        << " :MOTD File is missing\r\n";
    return oss.str();
}

// RPL Channel List Messages
inline std::string RPL_LIST(Client* client, const std::string& channel, int numVisible, const std::string& topic) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 322 " << CLIENT_FD(client) << " " << channel
        << " " << numVisible << " :" << topic << "\r\n";
    return oss.str();
}

inline std::string RPL_LISTEND(Client* client) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 323 " << CLIENT_FD(client)
        << " :End of /LIST\r\n";
    return oss.str();
}

//RPL Password
// RPL Error Messages
inline std::string ERR_NEEDMOREPARAMS(Client* client, const std::string& command)
{
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 461 " << CLIENT_FD(client) << " " << command << " :Not enough parameters\r\n";
    return oss.str();
}

inline std::string ERR_ALREADYREGISTERED(Client* client)
{
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 462 " << CLIENT_FD(client) << " :You may not reregister\r\n";
    return oss.str();
}

inline std::string ERR_PASSWDMISMATCH(Client* client)
{
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 464 " << CLIENT_FD(client) << " :Password incorrect\r\n";
    return oss.str();
}

inline std::string ERR_NOTREGISTERED(Client* client)
{
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 451 " << CLIENT_FD(client) << " :You have not registered\r\n";
    return oss.str();
}

inline std::string ERR_NONICKNAMEGIVEN(Client* client)
{
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 431 " << CLIENT_FD(client) << " :No nickname given\r\n";
    return oss.str();
}

inline std::string ERR_ERRONEUSNICKNAME(Client* client, const std::string& nickname)
{
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 432 " << CLIENT_FD(client) << " " << nickname << " :Erroneous nickname\r\n";
    return oss.str();
}

inline std::string ERR_NICKNAMEINUSE(Client* client, const std::string& nickname)
{
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 433 " << CLIENT_FD(client) << " " << nickname << " :Nickname is already in use\r\n";
    return oss.str();
}

// RPL Error Messages
inline std::string ERR_NOORIGIN(Client* client)
{
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 409 " << CLIENT_FD(client) << " :No origin specified\r\n";
    return oss.str();
}

// PONG Reply
inline std::string RPL_PONG(const std::string& token)
{
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " PONG " << SERVER_NAME << " " << token << "\r\n";
    return oss.str();
}

// ERR_UNKNOWNCOMMAND (421)
inline std::string ERR_UNKNOWNCOMMAND(Client* client, const std::string& command)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 421 " << CLIENT_FD(client) << " " << command << " :Unknown command\r\n";
	return oss.str();
}

// RPL Channel List Messages
inline std::string RPL_LIST(int clientFd, const std::string& channel, int numVisible, const std::string& topic)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 322 " << clientFd << " " << channel << " " << numVisible << " :" << topic << "\r\n";
	return oss.str();
}

inline std::string RPL_LISTEND(int clientFd)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 323 " << clientFd << " :End of /LIST\r\n";
	return oss.str();
}

inline std::string ERR_NEEDMOREPARAMS(int clientFd, const std::string& command)
{
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 461 " << clientFd << " " << command << " :Not enough parameters\r\n";
    return oss.str();
}

inline std::string ERR_UNKNOWNCOMMAND(int clientFd, const std::string& command)
{
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 421 " << clientFd << " " << command << " :Unknown command\r\n";
    return oss.str();
}


#endif // RPL_HPP
