#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <miniaudio.h>

int initAudio();
void shutdownAudio();
int play_sound(int id, int group, float volume, int looping);

#endif //__AUDIO_H__
