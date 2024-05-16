/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:17:42 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/16 18:18:19 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd)
	: _fd(fd), _authenticated(false)
{
}

void Client::setPassword(const std::string &password)
{
	_password = password;
}

const std::string &Client::getPassword() const
{
	return _password;
}

void Client::setNickname(const std::string &nickname)
{
	_nickname = nickname;
}

const std::string &Client::getNickname() const
{
	return _nickname;
}

void Client::setUser(const std::string &user)
{
	_user = user;
}

const std::string &Client::getUser() const
{
	return _user;
}

void Client::setRealName(const std::string &realname)
{
	_realname = realname;
}

const std::string &Client::getRealName() const
{
	return _realname;
}

bool Client::isAuthenticated() const
{
	return _authenticated;
}

void Client::authenticate()
{
	_authenticated = true;
}

int Client::getFd() const
{
	return _fd;
}




/*Client::Client(int fd)

Description: Constructeur de la classe Client. Initialise le client avec le descripteur de fichier fourni.
Paramètres:
int fd: Le descripteur de fichier du client.
Client::~Client()

Description: Destructeur de la classe Client. Gère la destruction de l'objet.
int Client::getFd() const

Description: Renvoie le descripteur de fichier du client.
Retourne: Le descripteur de fichier du client.
void Client::setPassword(const std::string &password)

Description: Définit le mot de passe du client et le marque comme authentifié.
Paramètres:
const std::string &password: Le mot de passe à définir.
bool Client::isAuthenticated() const

Description: Vérifie si le client est authentifié.
Retourne: true si le client est authentifié, false sinon.
void logMessage(const std::string &message, const std::string &color)

Description: Enregistre un message dans les logs et l'affiche avec des couleurs dans la console.
Paramètres:
const std::string &message: Le message à enregistrer.
const std::string &color: La couleur du message.*/