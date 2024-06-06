/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Welcome.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 19:53:17 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/06/01 19:02:40 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WELCOME_HPP
#define WELCOME_HPP

#include "Client.hpp"
#include "Server.hpp"
#include "Utils.hpp"

#include <string>

class WelcomeHandler
{
	public:
		void sendWelcomeMessages(Client *client, Server *server);
		void sendMotd(Client *client, Server *server);
};

#endif
