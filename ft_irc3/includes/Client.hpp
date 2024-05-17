/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:15:42 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/17 17:21:39 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
public:
    Client(int fd, const std::string &nickname, const std::string &user);

    int getFd() const;
    const std::string &getNickname() const;
    void setNickname(const std::string &nickname);
    const std::string &getUser() const;
    void setUser(const std::string &user);
    const std::string &getPassword() const;
    void setPassword(const std::string &password);
    const std::string &getRealName() const;
    void setRealName(const std::string &realname);
    bool isAuthenticated() const;
    void authenticate();
    bool isOperator() const;
    void setOperator(bool isOperator);

private:
    int _fd;
    std::string _nickname;
    std::string _user;
    std::string _password;
    std::string _realname;
    bool _authenticated;
    bool _operator;
};

#endif
