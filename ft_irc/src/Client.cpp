/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:07:27 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/13 16:59:32 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <cstring>
#include <sys/socket.h>

Client::Client() {}

Client::Client(int socket, const std::string& nickname, const std::string& username)
    : socket(socket), nickname(nickname), username(username) {}

Client::~Client() {
    close(socket);
}

void Client::sendMessage(const std::string& message) {
    std::cout << "Sending message: " << message << std::endl;
    send(socket, message.c_str(), message.size(), 0);
}

std::string Client::receiveMessage() {
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    int len = recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (len > 0) {
        buffer[len] = '\0';
        std::string received(buffer);
        received.erase(std::remove(received.begin(), received.end(), '\r'), received.end());
        received.erase(std::remove(received.begin(), received.end(), '\n'), received.end());
        std::cout << "Received message: [" << received << "]" << std::endl;
        return received;
    } else if (len == 0) {
        std::cout << "Connection closed by the peer." << std::endl;
    } else {
        std::cerr << "recv error: " << strerror(errno) << std::endl;
    }
    return "";
}


void Client::setNickname(const std::string& newNickname) {
    nickname = newNickname;
}

std::string Client::getNickname() const {
    return nickname;
}

int Client::getSocket() const {
    return socket;
}

void Client::setUsername(const std::string& newUsername) {
    nickname = newUsername;
}

void Client::setRealname(const std::string& newRealname) {
    // Assurez-vous d'avoir un membre `realname` dans Client
    username = newRealname;
}