/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TopicHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 17:04:33 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/06/01 19:02:10 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOPIC_HANDLER_HPP
#define TOPIC_HANDLER_HPP

#include <string>
#include <vector>
#include <sstream>
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Utils.hpp"


class TopicHandler
{
	public:
		TopicHandler(Server* server);
		void handleTopicCommand(Client* client, const std::string& command);

	private:
		void viewTopic(Client* client, Channel* channel);
		void changeTopic(Client* client, Channel* channel, const std::string& topic);

		Server* _server;
};

#endif
