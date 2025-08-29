NAME := minisynth

# Sources
PARSING		:= pars
MAIN		:= main
SOUNDS		:= synthesizer schedule wav

# Paths
OBJS_PATH	:= ./obj
DEP_PATH	:= ./dep
PORTAUDIO_DIR := ./libs/portaudio
PORTAUDIO_LIB := $(PORTAUDIO_DIR)/lib/.libs/libportaudio.a

# Source files
SRCS	:= $(addsuffix .c, $(addprefix src/, $(MAIN))) \
			$(addsuffix .c, $(addprefix src/parsing/, $(PARSING))) \
			$(addsuffix .c, $(addprefix src/sounds/, $(SOUNDS)))

# Objects
OBJ := $(SRCS:src/%=$(OBJS_PATH)/%)
OBJ := $(OBJ:.c=.o)

# Dependency files
DEP := $(SRCS:src/%=$(DEP_PATH)/%)
DEP := $(DEP:.c=.d)

# Compiler & flags
CC := cc
CFLAGS := -Wall -Wextra -Werror
HEADERS := -I./inc -I./libs/portaudio/include
LIBS := -lasound -lm -lpthread

all: $(NAME)

$(NAME): $(PORTAUDIO_LIB) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(PORTAUDIO_LIB) $(LIBS) -o $(NAME)

$(DEP_PATH)/%.d:
	@mkdir -p $(dir $@)

$(OBJS_PATH)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(HEADERS) -c $< -o $@

$(PORTAUDIO_LIB):
	@cd $(PORTAUDIO_DIR) && ./configure && make  > /dev/null 2>&1
	@echo "PortAudio library builded"

-include $(DEP)

clean:
	@rm -rf $(OBJS_PATH) $(OBJ) $(DEP)

fclean: clean
	@rm -f *.wav
	@rm -f $(NAME)
	@cd $(PORTAUDIO_DIR) && make clean > /dev/null 2>&1
	@echo "PortAudio library cleaned"
	@echo "Minissynthesizer cleaned"

re: fclean all

.PHONY: all clean fclean re