#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <miniaudio.h>

int initAudio();
void shutdownAudio();
int play_sound(int id, int group, float volume, int looping);
void set_global_volume(float volume);
void set_group_volume(int group, float volume);
void stop_all_sounds();
void stop_group_sounds(int group);

#endif //__AUDIO_H__
