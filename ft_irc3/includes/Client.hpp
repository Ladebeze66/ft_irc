/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:15:42 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/28 14:17:16 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
public:
	Client(int fd, const std::string &nickname, const std::string &user, const std::string &host, const std::string &password, const std::string &realname);

	int getFd() const;
	const std::string &getNickname() const;
	void setNickname(const std::string &nickname);
	const std::string &getUser() const;
	void setUser(const std::string &user);
	const std::string &getHost() const;
	void setHost(const std::string &host);
	const std::string &getPassword() const;
	void setPassword(const std::string &password);
	const std::string &getRealName() const;
	void setRealName(const std::string &realname);
	bool isAuthenticated() const;
	void authenticate();
	bool isOperator() const;
	void setOperator(bool isOperator);
	
	// Ajout des méthodes pour la gestion du statut "away"
	bool isAway() const;
	const std::string &getAwayMessage() const;
	void setAwayMessage(const std::string &message);
	void setAway(bool away);

private:
	int _fd;
	std::string _nickname;
	std::string _user;
	std::string _host;
	std::string _password;
	std::string _realname;
	bool _authenticated;
	bool _operator;
	bool _away;
	std::string _awayMessage;
};

#endif // CLIENT_HPP

