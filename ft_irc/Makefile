# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fgras-ca <fgras-ca@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/09 13:54:36 by fgras-ca          #+#    #+#              #
#    Updated: 2024/05/12 18:19:36 by fgras-ca         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Nom de l'exécutable à générer
NAME := ircserv

# Compilateur C++ et options de compilation
CXX := g++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98

# Dossiers
SRC_DIR := src
OBJ_DIR := build
INC_DIR := include

# Trouver les fichiers source (.cpp) et les transformer en fichiers objet (.o)
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Inclure les headers du dossier 'include'
CPPFLAGS := -I$(INC_DIR)

# L'exécutable final dépend des fichiers objet
$(NAME): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

# Compilation des fichiers source en fichiers objet
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -g -c $< -o $@

# Commande pour nettoyer le projet (supprimer les fichiers objet et l'exécutable)
clean:
	@rm -rf $(OBJ_DIR)

# Commande pour supprimer tout ce qui a été construit, reconstruire tout
fclean: clean
	@rm -f $(NAME)

# Commande pour recompiler tout le projet
re: fclean $(NAME)

# Éviter les conflits de noms avec des fichiers
.PHONY: clean fclean re
