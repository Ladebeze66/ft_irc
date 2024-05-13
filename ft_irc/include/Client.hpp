/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:07:00 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/13 17:48:53 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "Server.hpp"



class Client {
public:
	Client();
    Client(int socket, const std::string& nickname = "defaultNickName", const std::string& username = "defaultUserName");
    ~Client();

    void sendMessage(const std::string& message);
    std::string receiveMessage();
    void setNickname(const std::string& newNickname);
    std::string getNickname() const; // Ajout de la méthode getNickname
    int getSocket() const;
	void setUsername(const std::string& newUsername);
	void setRealname(const std::string& newRealname);

private:

    int socket;
    std::string nickname;  // Assurez-vous que cette donnée membre est accessible via getNickname
    std::string username;
};

#endif
