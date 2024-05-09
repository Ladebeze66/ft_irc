/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:07:00 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/09 14:33:20 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

class Client {
public:
    Client(int socket, const std::string& nickname, const std::string& username);
    ~Client();

    void sendMessage(const std::string& message);
    std::string receiveMessage();
    void setNickname(const std::string& newNickname);  // Ajout de la méthode setNickname

private:
    int socket;
    std::string nickname;
    std::string username;
};

#endif
