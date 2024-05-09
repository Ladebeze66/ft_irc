/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilities.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:22:54 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/09 14:35:04 by fgras-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <sstream>
#include <algorithm>
#include <cctype>
#include <locale>
#include <string>
#include <vector>

namespace Utilities {
    // Split a string by a delimiter and return a vector of substrings
    std::vector<std::string> split(const std::string& s, char delimiter);

    // Trim whitespace from the start and end of a string
    std::string trim(const std::string& s);
}

#endif // UTILITIES_HPP
