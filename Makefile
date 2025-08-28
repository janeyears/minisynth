NAME := minisynth

PARSING		:= 
MAIN		:= main errors
SOUNDS		:= 

SRCS	:= $(addsuffix .c, $(addprefix mandatory/src/, $(MAIN))) \
			$(addsuffix .c, $(addprefix mandatory/src/parsing/, $(PARSING))) \
			$(addsuffix .c, $(addprefix mandatory/src/game/, $(SOUNDS)))

OBJS := $(SRCS:mandatory/src/%=$(OBJS_PATH)/%)
OBJS := $(OBJS:.c=.o)



OBJ := $(SRC:.cpp=.o)
DEP := $(SRC:.cpp=.d)

CC := c++
CFLAGS := -Wall -Wextra -Werror
HEADERS := -I.

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) $(HEADERS) -MMD -MP -c $< -o $@

-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re