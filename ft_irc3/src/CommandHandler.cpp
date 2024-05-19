/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 18:26:34 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/19 19:13:14 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

CommandHandler::CommandHandler(Server *server)
    : _server(server)
{
}

void CommandHandler::handleCommand(Client* client, const std::string& command)
{
    std::vector<std::string> tokens = split(command, " \n\r\t");

    if (tokens.empty()) {
        return;
    }

    std::string commandType = tokens[0];
    if (commandType == "CAP") {
        handleCapCommand(client, tokens);
    } else if (commandType == "PASS") {
        handlePassCommand(client, tokens);
    } else if (commandType == "NICK") {
        handleNick(client, tokens);
    } else if (commandType == "USER") {
        handleUser(client, tokens);
    } else {
        if (!client->isAuthenticated()) {
            _server->sendToClient(client->getFd(), ERR_NOTREGISTERED(client));
            _server->log("Client " + client->getNickname() + " attempted to send a command before registering.", RED);
        } else {
            processCommand(client, command);
        }
    }
    std::cout << "Client " << client->getFd() << " " << client->getNickname() << " " << client->getUser() << " " << client->getPassword() << " " << client->getRealName() << std::endl;
}

void CommandHandler::handleCapCommand(Client* client, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2)
    {
        _server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client->getFd(), "CAP"));
        return;
    }

    std::string subcommand = tokens[1];

    if (subcommand == "LS")
    {
        // Example for CAP LS response
        std::vector<std::string> capLines;
        capLines.push_back("multi-prefix extended-join account-notify batch invite-notify tls");
        capLines.push_back("cap-notify server-time example.org/dummy-cap=dummyvalue example.org/second-dummy-cap");
        capLines.push_back("userhost-in-names sasl=EXTERNAL,DH-AES,DH-BLOWFISH,ECDSA-NIST256P-CHALLENGE,PLAIN");

        for (size_t i = 0; i < capLines.size(); ++i)
        {
            std::ostringstream oss;
            oss << ":irc.example.com CAP " << client->getNickname() << " LS";
            if (i != capLines.size() - 1)
                oss << " * :";
            else
                oss << " :";
            oss << capLines[i] << "\r\n";
            _server->sendToClient(client->getFd(), oss.str());
        }
    }
    else if (subcommand == "LIST")
    {
        // Example for CAP LIST response
        std::ostringstream oss;
        oss << ":irc.example.com CAP " << client->getNickname() << " LIST :example.org/example-cap example.org/second-example-cap account-notify invite-notify batch example.org/third-example-cap\r\n";
        _server->sendToClient(client->getFd(), oss.str());
    }
    else if (subcommand == "REQ")
    {
        if (tokens.size() < 3)
        {
            _server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client->getFd(), "CAP"));
            return;
        }

        // Example for CAP REQ response
        std::string requestedCaps = tokens[2];
        std::ostringstream ackResponse;
        ackResponse << ":irc.example.com CAP " << client->getNickname() << " ACK :" << requestedCaps << "\r\n";
        _server->sendToClient(client->getFd(), ackResponse.str());
    }
    else if (subcommand == "END")
    {
        // Example for CAP END response
        _server->sendToClient(client->getFd(), ":irc.example.com CAP " + client->getNickname() + " END\r\n");
    }
    else
    {
        // Unknown CAP subcommand
        _server->sendToClient(client->getFd(), ERR_UNKNOWNCOMMAND(client->getFd(), "CAP"));
    }
}

void CommandHandler::handlePassCommand(Client* client, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2) {
        _server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "PASS"));
        return;
    }

    if (client->isAuthenticated()) {
        _server->sendToClient(client->getFd(), ERR_ALREADYREGISTERED(client));
        return;
    }

    if (tokens[1] == _server->_password) {
        client->setPassword(tokens[1]);
        _server->sendToClient(client->getFd(), ":server NOTICE * :Password accepted\r\n");
        _server->log("Client " + client->getNickname() + " provided correct password.", GREEN);
    } else {
        _server->sendToClient(client->getFd(), ERR_PASSWDMISMATCH(client));
        _server->log("Client " + client->getNickname() + " failed authentication password.", RED);
        // Optionally disconnect the client here
    }
}

bool CommandHandler::isValidNickname(const std::string& nickname)
{
    // Implement nickname validation according to IRC protocol
    if (nickname.empty() || nickname[0] == '#' || nickname[0] == ':' || nickname.find(' ') != std::string::npos) {
        return false;
    }
    return true;
}

bool CommandHandler::isNicknameInUse(const std::string& nickname)
{
    for (std::map<int, Client*>::iterator it = _server->_clients.begin(); it != _server->_clients.end(); ++it) {
        if (it->second->getNickname() == nickname) {
            return true;
        }
    }
    return false;
}

void CommandHandler::handleNick(Client* client, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2) {
        _server->sendToClient(client->getFd(), ERR_NONICKNAMEGIVEN(client));
        return;
    }

    std::string newNick = tokens[1];

    if (!isValidNickname(newNick)) {
        _server->sendToClient(client->getFd(), ERR_ERRONEUSNICKNAME(client, newNick));
        return;
    }

    if (isNicknameInUse(newNick)) {
        _server->sendToClient(client->getFd(), ERR_NICKNAMEINUSE(client, newNick));
        return;
    }

    std::string oldNick = client->getNickname();
    client->setNickname(newNick);

    if (!oldNick.empty()) {
        _server->broadcast(":" + oldNick + " NICK " + newNick + "\r\n");
    }

    _server->sendToClient(client->getFd(), ":" + newNick + " NICK " + newNick + "\r\n");
    _server->log("Client " + oldNick + " changed nickname to " + newNick, GREEN);
}

void CommandHandler::handleUser(Client* client, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 5) {
        _server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "USER"));
        return;
    }

    if (client->isAuthenticated()) {
        _server->sendToClient(client->getFd(), ERR_ALREADYREGISTERED(client));
        return;
    }

    client->setUser(tokens[1]);
    client->setRealName(tokens[4].substr(1)); // remove leading ':'

    if (client->getPassword() == _server->_password && !client->getNickname().empty()) {
        client->authenticate();
        sendWelcomeMessages(client, _server);
        _server->log("Client " + client->getNickname() + " authenticated.", GREEN);
    }
}

void CommandHandler::processCommand(Client *client, const std::string &command)
{
    if (command.find("JOIN") == 0)
    {
        std::string channelName = command.substr(5);
        handleJoinCommand(client, channelName);
    }
    else if (command.find("PART") == 0)
    {
        handlePartCommand(_server, client, command);
    }
    else if (command.find("NICK") == 0)
    {
        handleNickCommand(_server, client, command);
    }
    else if (command.find("PRIVMSG") == 0)
    {
        handlePrivmsgCommand(_server, client, command);
    }
    else if (command.find("MODE") == 0)
    {
        ModeWhoHandler modeHandler(_server);
        modeHandler.handleModeCommand(client, command);
    }
    else if (command.find("WHO") == 0)
    {
        ModeWhoHandler whoHandler(_server);
        whoHandler.handleWhoCommand(client, command);
    }
	else if (command.find("PING") == 0)
    {
        std::vector<std::string> tokens = split(command, " ");
        handlePingCommand(client, tokens);
    }
	else if (command.find("LIST") == 0)
    {
        broadcastChannelList(client, _server);
    }
    else
    {
        _server->sendToClient(client->getFd(), ERR_UNKNOWNCOMMAND(client, command));
        _server->log("Message from client " + client->getNickname() + ": " + command, MAGENTA);
    }
}

void CommandHandler::handleJoinCommand(Client *client, const std::string &channelName)
{
    if (_server->getChannels().find(channelName) == _server->getChannels().end())
    {
        _server->getChannels()[channelName] = new Channel(channelName);
        _server->log("Channel created: " + channelName, GREEN);

        // Notifier tous les clients de la création du canal
        _server->broadcast(":server NOTICE * :New channel created: " + channelName + "\r\n");
    }
    Channel *channel = _server->getChannels()[channelName];
    channel->addClient(client);

    std::stringstream joinMsg;
    joinMsg << ":" << client->getNickname() << " JOIN :" << channelName << "\r\n";
    _server->sendToClient(client->getFd(), joinMsg.str());

    std::string usersList = getUsersList(channel);
    _server->sendToClient(client->getFd(), usersList);

    std::stringstream endOfNamesMsg;
    endOfNamesMsg << ":server 366 " << client->getNickname() << " " << channelName << " :End of /NAMES list.\r\n";
    _server->sendToClient(client->getFd(), endOfNamesMsg.str());

    std::stringstream ss;
    ss << "Client " << client->getNickname() << " joined channel " << channelName;
    _server->log(ss.str(), MAGENTA);
}

std::string CommandHandler::getUsersList(Channel *channel)
{
    std::vector<Client *> clients = channel->getClients();
    std::stringstream ss;
    ss << ":server 353 " << clients[0]->getNickname() << " = " << channel->getName() << " :";
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (i > 0)
            ss << " ";
        ss << clients[i]->getNickname();
    }
    ss << "\r\n";
    return ss.str();
}

void CommandHandler::handlePingCommand(Client* client, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2) {
        _server->sendToClient(client->getFd(), ERR_NEEDMOREPARAMS(client, "PING"));
        return;
    }

    std::string token = tokens[1];
    _server->sendToClient(client->getFd(), RPL_PONG(token));
}
