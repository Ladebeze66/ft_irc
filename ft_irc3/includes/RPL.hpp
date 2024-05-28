/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPL.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 15:12:47 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/28 15:26:33 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPL_HPP
#define RPL_HPP

#include <string>
#include <sstream>
#include "Client.hpp"

#define SERVER_NAME "IRC_Server"
#define SERVER_VERSION "1.0"
#define USER_MODES ""
#define CHANNEL_MODES ""
#define CHANNEL_MODES_WITH_PARAMS ""



// Macros pour accéder aux champs du client
#define CLIENT_FD(client) (client->getFd())
#define CLIENT_NICK(client) ((client)->getNickname())
#define CLIENT_USER(client) ((client)->getUser())
#define CLIENT_HOST(client) ((client)->getHost())
#define CLIENT_REALNAME(client) ((client)->getRealName())

// Fonctions pour générer les réponses RPL
inline std::string RPL_WELCOME(Client* client) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 001 " << CLIENT_NICK(client)
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

inline std::string RPL_UMODEIS(int clientFd, const std::string& modes) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 221 " << clientFd << " :" << modes << "\r\n";
	return oss.str();
}

inline std::string RPL_AWAY(int clientFd, const std::string& target, const std::string& message) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 301 " << clientFd << " " << target << " :" << message << "\r\n";
	return oss.str();
}

// WHOIS Command RPLs
inline std::string RPL_WHOISUSER(int clientFd, Client* target)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 311 " << clientFd << " " << CLIENT_NICK(target) << " "
		<< CLIENT_USER(target) << " " << CLIENT_HOST(target) << " * :" << CLIENT_REALNAME(target) << "\r\n";
	return oss.str();
}

inline std::string RPL_WHOISSERVER(int clientFd, const std::string& targetNick, const std::string& serverInfo)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 312 " << clientFd << " " << targetNick << " " << SERVER_NAME
		<< " :" << serverInfo << "\r\n";
	return oss.str();
}

inline std::string RPL_ENDOFWHO(int clientFd, const std::string& channel)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 315 " << clientFd << " " << channel << " :End of /WHO list.\r\n";
	return oss.str();
}

inline std::string RPL_ENDOFWHOIS(int clientFd, const std::string& targetNick)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 318 " << clientFd << " " << targetNick << " :End of /WHOIS list.\r\n";
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

// RPL Mode Messages
inline std::string RPL_CHANNELMODEIS(int clientFd, const std::string& channel, const std::string& mode)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 324 " << clientFd << " " << channel << " " << mode << "\r\n";
	return oss.str();
}

inline std::string RPL_CREATIONTIME(int clientFd, const std::string& channel, time_t creationTime) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 329 " << clientFd << " " << channel << " " << creationTime << "\r\n";
	return oss.str();
}

inline std::string RPL_NOTOPIC(Client* client, const std::string& channel) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 331 " << CLIENT_NICK(client) << " " << channel
        << " :No topic is set\r\n";
    return oss.str();
}

inline std::string RPL_TOPIC(Client* client, const std::string& channel, const std::string& topic) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 332 " << CLIENT_NICK(client) << " " << channel << " :" << topic << "\r\n";
	return oss.str();
}

inline std::string RPL_TOPICWHOTIME(Client* client, const std::string& channel, const std::string& setter, time_t setTime) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 333 " << CLIENT_NICK(client) << " " << channel << " " << setter << " " << setTime << "\r\n";
	return oss.str();
}

inline std::string RPL_INVITELIST(int clientFd, const std::string& channel, const std::string& inviteMask) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 336 " << clientFd << " " << channel << " " << inviteMask << "\r\n";
	return oss.str();
}

inline std::string RPL_ENDOFINVITELIST(int clientFd, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 337 " << clientFd << " " << channel << " :End of channel invite list\r\n";
	return oss.str();
}

// Ajoutez cette fonction pour RPL_INVITING
inline std::string RPL_INVITING(Client* client, const std::string& channel, const std::string& nick) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 341 " << CLIENT_NICK(client) << " " << nick << " " << channel << "\r\n";
    return oss.str();
}

inline std::string RPL_EXCEPTLIST(int clientFd, const std::string& channel, const std::string& exceptionMask) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 348 " << clientFd << " " << channel << " " << exceptionMask << "\r\n";
	return oss.str();
}

inline std::string RPL_ENDOFEXCEPTLIST(int clientFd, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 349 " << clientFd << " " << channel << " :End of channel exception list\r\n";
	return oss.str();
}

inline std::string RPL_NAMREPLY(Client* client, const std::string& channel, const std::string& users) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 353 " << CLIENT_NICK(client) << " = " << channel << " :" << users << "\r\n";
	return oss.str();
}
// WHO Command RPLs
inline std::string RPL_WHOREPLY(const std::string& channel, Client* target)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 352 " << CLIENT_NICK(target) << " " << channel << " "
		<< CLIENT_USER(target) << " " << CLIENT_HOST(target) << " " << SERVER_NAME << " "
		<< CLIENT_NICK(target) << " H :0 " << CLIENT_REALNAME(target) << "\r\n";
	return oss.str();
}

inline std::string RPL_ENDOFNAMES(Client* client, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 366 " << CLIENT_NICK(client) << " " << channel << " :End of /NAMES list\r\n";
	return oss.str();
}

inline std::string RPL_BANLIST(int clientFd, const std::string& channel, const std::string& banMask) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 367 " << clientFd << " " << channel << " " << banMask << "\r\n";
	return oss.str();
}

inline std::string RPL_ENDOFBANLIST(int clientFd, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 368 " << clientFd << " " << channel << " :End of channel ban list\r\n";
	return oss.str();
}

inline std::string RPL_MOTD(Client* client, const std::string& line) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 372 " << CLIENT_FD(client)
		<< " :- " << line << "\r\n";
	return oss.str();
}

inline std::string RPL_MOTDSTART(Client* client) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 375 " << CLIENT_FD(client)
		<< " :- " << SERVER_NAME << " Message of the day - \r\n";
	return oss.str();
}

inline std::string RPL_ENDOFMOTD(Client* client) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 376 " << CLIENT_FD(client)
		<< " :End of /MOTD command.\r\n";
	return oss.str();
}

inline std::string ERR_NOSUCHNICK(int clientFd, const std::string& nick)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 401 " << clientFd << " " << nick << " :No such nick/channel\r\n";
	return oss.str();
}

inline std::string ERR_NOSUCHCHANNEL(int clientFd, const std::string& channel)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 403 " << clientFd << " " << channel << " :No such channel\r\n";
	return oss.str();
}

inline std::string ERR_CANNOTSENDTOCHAN(int clientFd, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 404 " << clientFd << " " << channel << " :Cannot send to channel\r\n";
	return oss.str();
}

inline std::string ERR_TOOMANYCHANNELS(Client* client, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 405 " << CLIENT_NICK(client) << " " << channel << " :You have joined too many channels\r\n";
	return oss.str();
}

// RPL Error Messages
inline std::string ERR_NOORIGIN(Client* client)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 409 " << CLIENT_FD(client) << " :No origin specified\r\n";
	return oss.str();
}

inline std::string ERR_NORECIPIENT(int clientFd, const std::string& command) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 411 " << clientFd << " :No recipient given (" << command << ")\r\n";
	return oss.str();
}

inline std::string ERR_NOTEXTTOSEND(int clientFd) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 412 " << clientFd << " :No text to send\r\n";
	return oss.str();
}

inline std::string ERR_UNKNOWNCOMMAND(Client* client, const std::string& command) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 421 " << CLIENT_NICK(client) << " " << command << " :Unknown command\r\n";
	return oss.str();
}

inline std::string ERR_NOMOTD(Client* client) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 422 " << CLIENT_FD(client)
		<< " :MOTD File is missing\r\n";
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

inline std::string ERR_USERNOTINCHANNEL(int clientFd, const std::string& nick, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 441 " << clientFd << " " << nick << " " << channel
		<< " :They aren't on that channel\r\n";
	return oss.str();
}

inline std::string ERR_NOTONCHANNEL(int clientFd, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 442 " << clientFd << " " << channel << " :You're not on that channel\r\n";
	return oss.str();
}

// Ajoutez cette fonction pour ERR_USERONCHANNEL
inline std::string ERR_USERONCHANNEL(Client* client, const std::string& nick, const std::string& channel) {
    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " 443 " << CLIENT_NICK(client) << " " << nick << " " << channel
        << " :is already on channel\r\n";
    return oss.str();
}

inline std::string ERR_NOTREGISTERED(Client* client)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 451 " << CLIENT_FD(client) << " :You have not registered\r\n";
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

inline std::string ERR_CHANNELISFULL(Client* client, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 471 " << CLIENT_NICK(client) << " " << channel << " :Cannot join channel (channel is full)\r\n";
	return oss.str();
}

inline std::string ERR_INVITEONLYCHAN(Client* client, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 473 " << CLIENT_NICK(client) << " " << channel << " :Cannot join channel (invite only)\r\n";
	return oss.str();
}

inline std::string ERR_BANNEDFROMCHAN(Client* client, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 474 " << CLIENT_NICK(client) << " " << channel << " :Cannot join channel (banned)\r\n";
	return oss.str();
}

inline std::string ERR_BADCHANNELKEY(Client* client, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 475 " << CLIENT_NICK(client) << " " << channel << " :Cannot join channel (incorrect key)\r\n";
	return oss.str();
}

inline std::string ERR_BADCHANMASK(Client* client, const std::string& channel) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 476 " << CLIENT_NICK(client) << " " << channel << " :Bad channel mask\r\n";
	return oss.str();
}

inline std::string RPL_NOCHANMODES(int clientFd, const std::string& channel)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 477 " << clientFd << " " << channel << " :Channel doesn't support modes\r\n";
	return oss.str();
}

inline std::string ERR_CHANOPRIVSNEEDED(int clientFd, const std::string& channel)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 482 " << clientFd << " " << channel << " :You're not channel operator\r\n";
	return oss.str();
}

inline std::string ERR_UMODEUNKNOWNFLAG(int clientFd) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 501 " << clientFd << " :Unknown MODE flag\r\n";
	return oss.str();
}

inline std::string ERR_USERSDONTMATCH(int clientFd) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " 502 " << clientFd << " :Cannot change mode for other users\r\n";
	return oss.str();
}

// PONG Reply
inline std::string RPL_PONG(const std::string& token)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " PONG " << SERVER_NAME << " " << token << "\r\n";
	return oss.str();
}

// CAP Command RPLs
inline std::string RPL_CAP(int clientFd, const std::string& subcommand, const std::string& capabilities)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " CAP " << clientFd << " " << subcommand << " :" << capabilities << "\r\n";
	return oss.str();
}

inline std::string RPL_PASSACCEPTED(Client* client)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " NOTICE " << CLIENT_FD(client)
		<< " :Password accepted!\r\n";
	return oss.str();
}

// Add this function to handle the CAP END response
inline std::string RPL_CAPEND(int clientFd)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " CAP " << clientFd << " END\r\n";
	return oss.str();
}

#endif // RPL_HPP
