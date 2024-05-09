/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:00:46 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/09 14:00:52 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    int port = std::atoi(argv[1]);
    std::string password = argv[2];

    Server ircServer(port, password);
    if (!ircServer.initialize()) {
        std::cerr << "Failed to initialize server." << std::endl;
        return 1;
    }
    ircServer.run();

    return 0;
}
