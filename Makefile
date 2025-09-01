PORTAUDIO_DIR	:= portaudio_minisynth
ALSA_DIR		:= alsa_minisynth

all:
	$(MAKE) -C $(PORTAUDIO_DIR)
	$(MAKE) -C $(ALSA_DIR)
	mv $(PORTAUDIO_DIR)/minisynth_portaudio .
	mv $(ALSA_DIR)/minisynth_alsa .

alsa:
	$(MAKE) -C $(ALSA_DIR)
	mv $(ALSA_DIR)/minisynth_alsa .

portaudio:
	$(MAKE) -C $(PORTAUDIO_DIR)
	mv $(PORTAUDIO_DIR)/minisynth_portaudio .

clean:
	$(MAKE) -C $(PORTAUDIO_DIR) clean
	$(MAKE) -C $(ALSA_DIR) clean

fclean:
	$(MAKE) -C $(PORTAUDIO_DIR) fclean
	$(MAKE) -C $(ALSA_DIR) fclean
	rm -f minisynth_portaudio minisynth_alsa

re: fclean all

.PHONY: all alsa portaudio clean fclean re
