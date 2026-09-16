#pragma once
#include <Arduino.h>
#include "app_config.h"

#if ENABLE_MODULE_AUDIO

bool audio_module_init();
void audio_play_sound(const char* sound_name);
void audio_start_listening();
void audio_stop_listening();

#endif // ENABLE_MODULE_AUDIO
