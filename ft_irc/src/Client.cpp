/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:07:27 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/09 14:33:11 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int socket, const std::string& nickname, const std::string& username)
    : socket(socket), nickname(nickname), username(username) {}

Client::~Client() {
    close(socket);
}

void Client::sendMessage(const std::string& message) {
    send(socket, message.c_str(), message.size(), 0);
}

std::string Client::receiveMessage() {
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    int len = recv(socket, buffer, sizeof(buffer), 0);
    if (len > 0) {
        return std::string(buffer, len);
    }
    return "";
}

void Client::setNickname(const std::string& newNickname) {
    nickname = newNickname;
}
