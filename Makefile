
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lminta <lminta@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: Invalid date        by                   #+#    #+#              #
#    Updated: 2019/11/29 21:06:38 by lminta           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = Scop
FLAGS = -v -g # -Wall -Wextra -Werror
CC = clang++

INCLUDES = -I./libs/glfw/include/ -I./libs/glad/include/ -I./include/
HEADERS_DIRECTORY = 
HEADERS_LIST = 
HEADERS = 

DIRECTORY =  $(shell pwd)
GLAD_DIRECTORY := $(DIRECTORY)/libs/glad/
GLAD := $(GLAD_DIRECTORY)libglad.a
GLFW_DIRECTORY := $(DIRECTORY)/libs/glfw/
GLFW := $(GLFW_DIRECTORY)src/libglfw3.a

SRCS_DIRECTORY = ./src/
SRCS_LIST = main.cpp

OBJS_DIRECTORY = objects/
OBJS_LIST = $(patsubst %.cpp, %.o, $(SRCS_LIST))
OBJS = $(addprefix $(OBJS_DIRECTORY), $(OBJS_LIST))
SDL_LIBS = $(addprefix $(DIRECTORY)/lib/, $(LIB_LIST))
MAKES = makes

GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m
CLEAR_LINE	:= \033[2K
BEGIN_LINE	:= \033[A
COL_END		:= \033[0m
COL_RED		:= \033[1;31m
COL_GREEN	:= \033[1;32m
COL_YELLOW	:= \033[1;33m
COL_BLUE	:= \033[1;34m
COL_VIOLET	:= \033[1;35m
COL_CYAN	:= \033[1;36m
COL_WHITE	:= \033[1;37m

TOTAL_FILES := $(shell echo $(SRCS_LIST) | wc -w | sed -e 's/ //g')
CURRENT_FILES = $(shell find $(DIRECTORY)/objects/ -type f 2> /dev/null | wc -l | sed -e 's/ //g')
START_FILES = $(shell find $(DIRECTORY)/objects/ -type f 2> /dev/null | wc -l | sed -e 's/ //g')
START_FILES_N := $(START_FILES)

ifeq ($(OS),Windows_NT)
	detected_OS := Windows
else
	detected_OS := $(shell uname)
endif

ifeq ($(detected_OS),Linux)
	LIBRARIES += -L./libs/glad/ -lglad -ldl  -lGL -L./libs/glfw/src/ -lglfw3 \
	-lXrandr -lXrender -lXi -lXfixes -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp
endif
ifeq ($(detected_OS),Darwin)  
	LIBRARIES = -L./libs/glad/ -lglad -L./libs/glfw/src/ -lglfw3      # Mac OS X
	LIBRARIES += -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
endif


.PHONY: clean fclean re

all: $(MAKES) $(NAME)


$(NAME): $(OBJS) $(HEADERS)  $(GLFW)
	echo $(GLFW)
	$(CC) $(FLAGS)  $(INCLUDES) $(OBJS)  -o $(NAME) $(LIBRARIES)
	@echo "$(CLEAR_LINE)[`expr $(CURRENT_FILES) '*' 100 / $(TOTAL_FILES) `%] $(COL_BLUE)[$(NAME)] $(COL_GREEN)Finished compilation. Output file : $(COL_VIOLET)$(PWD)/$(NAME)$(COL_END)"

$(MAKES):
	@$(MAKE) -sC $(GLAD_DIRECTORY)

$(OBJS_DIRECTORY):
	@mkdir -p $(OBJS_DIRECTORY)


$(OBJS_DIRECTORY)%.o : $(SRCS_DIRECTORY)%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -c $(INCLUDES) $< -o $@
	@echo "$(CLEAR_LINE)[`expr $(CURRENT_FILES) '*' 100 / $(TOTAL_FILES) `%] $(COL_BLUE)[$(NAME)] $(COL_GREEN)Compiling file [$(COL_VIOLET)$<$(COL_GREEN)].($(CURRENT_FILES) / $(TOTAL_FILES))$(COL_END)$(BEGIN_LINE)"


$(GLFW):
	@cd libs/glfw/ && cmake . && make glfw; 
sub:
	git submodule update --init --recursive;
this:
	@rm -rf $(OBJS_DIRECTORY) && make;

clean:
	@rm -rf $(OBJS_DIRECTORY)
	@echo "$(NAME): $(RED)$(OBJECTS_DIRECTORY) was deleted$(RESET)"
	@echo "$(NAME): $(RED)object files were deleted$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(NAME): $(RED)$(NAME) was deleted$(RESET)"

re:
	@$(MAKE) fclean
	@$(MAKE) all
