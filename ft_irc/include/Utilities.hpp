/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utilities.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:22:54 by fgras-ca          #+#    #+#             */
/*   Updated: 2024/05/11 13:34:54 by fgras-ca         ###   ########.fr       */
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
    std::vector<std::string> split(const std::string& str, char delimiter);
}

#endif // UTILITIES_HPP
