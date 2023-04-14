# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/14 09:52:20 by sunhwang          #+#    #+#              #
#    Updated: 2023/04/14 09:58:09 by sunhwang         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = miniRT

OUT_DIR		= out/

DIR_SRC		= src
DIR_ERROR	= error
DIR_PARSE	= parse

SRC_DIRS	= $(sort $(addprefix $(DIR_SRC)/, $(DIR_ERROR) $(DIR_PARSE)))
SRC_INC_DIR	= $(addprefix -I, $(SRC_DIRS))
INC_DIR		= -Ilib/libft -Ilib/mlx -I$(DIR_SRC) $(SRC_INC_DIR)

SRC_ERROR	= common_error
SRC_PARSE	= parse_config parse_request

ifeq (,$(findstring bonus,$(MAKECMDGOALS)))
# SRC_WINDOW	+= draw_rt
else
# SRC_WINDOW	+= draw_rt_bonus
endif

SRCS_LIST = $(addprefix $(DIR_SRC)/,					\
			$(addprefix $(DIR_ERROR)/,	$(SRC_ERROR))	\
			$(addprefix $(DIR_PARSE)/,	$(SRC_PARSE)))	
	
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
# LDFLAGS	= 

SRCS = $(addsuffix .cpp, $(DIR_SRC)/main $(SRCS_LIST))
OBJS = $(SRCS:%.cpp=$(OUT_DIR)%.o)

all: $(NAME)

bonus: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

clean:
	$(RM) -r $(OUT_DIR)

fclean: clean
	$(RM) $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

$(OBJS): $(OUT_DIR)%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_DIR) -c $< -o $@

.PHONY: all clean fclean re