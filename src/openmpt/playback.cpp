/*
 * Playback.cpp
 * -------------
 * Purpose: Playback functions for the vitaTrak application.
 * Notes  : This platform uses SCE functions for audio playback.
 */

#include "openmpt/playback.h"
#include <exception>
#include <fstream>
#include <new>
#include <stdexcept>
#include <iostream>
#include <psp2/audioout.h>
#include <vita2d.h>
#include <psp2/ctrl.h>
#include <main.h>

// thread management
#include <psp2/kernel/threadmgr.h>

// libopenmpt 0.5.24 headers
#include <libopenmpt/libopenmpt.hpp>
#include <libopenmpt/libopenmpt_ext.hpp>
#include <libopenmpt/libopenmpt_stream_callbacks_file.h>

// TODO: allow for changing these values?
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_CHANNELS 2
#define AUDIO_FRAME_SIZE 64


int musicPlaybackThread(unsigned int args, void *arg) {
    std::string playback_status;
    // initialize vitaAudio
    sceAudioOutSetConfig(SCE_AUDIO_OUT_PORT_TYPE_MAIN, AUDIO_CHANNELS, AUDIO_SAMPLE_RATE, SCE_AUDIO_OUT_MODE_STEREO);
    SceUID audio_out = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_MAIN, AUDIO_FRAME_SIZE, AUDIO_SAMPLE_RATE, SCE_AUDIO_OUT_MODE_STEREO);

    printf("moment of truth...\n");
    char *voidToString = static_cast<char*>(arg);
    // print arguments
    printf("args: %d\n", args);
    printf("arg: %s\n", voidToString);

    try {
        // file path is passed through argument
        std::ifstream file( voidToString, std::ios::binary);
        openmpt::module mod(file);
        mod.set_repeat_count(0);
        printf("loading module...\n");
        // Retrieve information about the module
        printf("module information:\n");
        printf("  type: %s\n", mod.get_metadata("type_long").c_str());
        printf("  title: %s\n", mod.get_metadata("title").c_str());

        // update status with proper title, if not available fall back to filename
        if (mod.get_metadata("title").c_str() != NULL) {
            playback_status = mod.get_metadata("title");
        }else{
            playback_status = voidToString;
        }

        // allocate audio buffer
        uint16_t *audio_buffer = new uint16_t[AUDIO_FRAME_SIZE * AUDIO_CHANNELS];
        // play the module
        printf("playing module...\n");
        //playback = true;
        for (size_t i = 0; i < mod.get_duration_seconds() * AUDIO_SAMPLE_RATE; i += AUDIO_FRAME_SIZE) {
            // vita audio output is S16 (do not use floating points! they will cause static noise.)
            mod.read_interleaved_stereo(AUDIO_SAMPLE_RATE, AUDIO_FRAME_SIZE, reinterpret_cast<int16_t *>(audio_buffer));
            sceAudioOutOutput(audio_out, audio_buffer);
            // check for input events (e.g. start button pressed)
            SceCtrlData pad;
            if (sceCtrlPeekBufferPositive(0, &pad, 1) > 0) {
                if (pad.buttons & SCE_CTRL_START) {
                    printf("playback stopped\n");
                    break;
                }
            }
        }
        printf("module play finished!\n");
        // close vitaAudio
        sceAudioOutReleasePort(audio_out);
        // free audio buffer
        delete[] audio_buffer;
        printf("playback done!\n");
        playback_status = "doing literally nothing2";
        return sceKernelExitDeleteThread(0);
    } catch (const std::exception &e) {
        // dump error
        printf("FUCK! an exception! Error: %s\n", e.what());
        // exit thread
        return sceKernelExitDeleteThread(1);
    }
}