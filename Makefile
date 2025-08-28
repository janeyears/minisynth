NAME := minisynth

PARSING		:= pars
MAIN		:= main
#SOUNDS		:= sounds
OBJS_PATH	:= ./obj
DEP_PATH	:= ./dep

SRCS	:= $(addsuffix .c, $(addprefix src/, $(MAIN))) \
			$(addsuffix .c, $(addprefix src/parsing/, $(PARSING)))
#			$(addsuffix .c, $(addprefix src/sounds/, $(SOUNDS)))

OBJ := $(SRCS:src/%=$(OBJS_PATH)/%)
OBJ := $(OBJ:.c=.o)

DEP := $(SRCS:src/%=$(DEP_PATH)/%)
DEP := $(DEP:.c=.d)

CC := cc
CFLAGS := -Wall -Wextra -Werror
HEADERS := -I ./inc

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(DEP_PATH)/%.d:
	@mkdir -p $(dir $@)

$(OBJS_PATH)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(HEADERS) -c $< -o $@

-include $(DEP)

clean:
	@rm -rf $(OBJS_PATH) $(OBJ) $(DEP)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re