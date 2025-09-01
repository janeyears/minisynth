NAME := minisynth_alsa

# Sources
PARSING		:= parsing parsing_utils
MAIN		:= main
SOUNDS		:= synthesizer schedule visualaizer

# Paths
OBJS_PATH	:= ./obj

# Source files
SRCS	:= $(addsuffix .c, $(addprefix src/, $(MAIN))) \
			$(addsuffix .c, $(addprefix src/parsing/, $(PARSING))) \
			$(addsuffix .c, $(addprefix src/sounds/, $(SOUNDS)))

# Objects
OBJ := $(SRCS:src/%=$(OBJS_PATH)/%)
OBJ := $(OBJ:.c=.o)

# Dependency files
DEP := $(OBJ:.o=.d)

# Compiler & flags
CC := cc
CFLAGS := -Wall -Wextra -Werror
HEADERS := -I./inc
LIBS := -lasound -lm -lpthread

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) -o $(NAME)

$(OBJS_PATH)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(HEADERS) -MMD -MP -c $< -o $@


-include $(DEP)

clean:
	@rm -rf $(OBJS_PATH) $(OBJ) $(DEP)

fclean: clean
	@rm -f $(NAME)
	@echo "Minisynthesizer cleaned"

re: fclean all

.PHONY: all clean fclean re
