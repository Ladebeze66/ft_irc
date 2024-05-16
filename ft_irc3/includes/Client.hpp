/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:15:42 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/16 18:18:02 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
public:
	Client(int fd);

	void setPassword(const std::string &password);
	const std::string &getPassword() const;

	void setNickname(const std::string &nickname);
	const std::string &getNickname() const;

	void setUser(const std::string &user);
	const std::string &getUser() const;

	void setRealName(const std::string &realname);
	const std::string &getRealName() const;

	bool isAuthenticated() const;
	void authenticate();

	int getFd() const;

private:
	int _fd;
	std::string _password;
	std::string _nickname;
	std::string _user;
	std::string _realname;
	bool _authenticated;
};

#endif
