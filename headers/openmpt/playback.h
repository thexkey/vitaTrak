/**
 * Playback.h
 * -------------
 * Purpose: OpenMPT playback functions for the vitaTrak application.
 * Notes  : This platform uses SCE functions for audio playback.
 *
 **/

#ifndef VITATRAK_PLAYBACK_H
#define VITATRAK_PLAYBACK_H

#include <psp2/audioout.h>


int musicPlaybackThread(unsigned int args, void *arg);


#endif //VITATRAK_PLAYBACK_H
