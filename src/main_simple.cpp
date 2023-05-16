/*
 * main.cpp
 * -------------
 * Purpose: Main entry point for the vitaTrak application.
 * Notes  : This platform uses SCE functions for audio playback.
 *
 */

#include <exception>
#include <fstream>
#include <new>
#include <stdexcept>
#include <iostream>


#include <psp2/audioout.h>
#include <vita2d.h>
#include <psp2/ctrl.h>

// libopenmpt 0.5.24 headers
#include <libopenmpt/libopenmpt.hpp>
#include <libopenmpt/libopenmpt_ext.hpp>
#include <libopenmpt/libopenmpt_stream_callbacks_file.h>

#define AUDIO_SAMPLE_RATE 48000
// unsure if this means Sony channels or OpenMPT channels.
#define AUDIO_CHANNELS 2
// do not change this value, it will cause static noise if higher than expected.
#define AUDIO_FRAME_SIZE 64

#include <imgui_vita2d/imgui_vita.h>
#include <stdio.h>
#include <vita2d.h>


int audioThread(SceSize args, void *argp) {
    sceAudioOutSetConfig(SCE_AUDIO_OUT_PORT_TYPE_MAIN, AUDIO_CHANNELS, AUDIO_SAMPLE_RATE, SCE_AUDIO_OUT_MODE_STEREO);
    SceUID audio_out = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_MAIN, AUDIO_FRAME_SIZE, AUDIO_SAMPLE_RATE, SCE_AUDIO_OUT_MODE_STEREO);

    printf("moment of truth...\n");
    std::ifstream file( "ux0:/data/vitaTrak/mysteries-edit.xm", std::ios::binary );
    openmpt::module mod(file);
    mod.set_repeat_count(0);
    printf("loading module...\n");

    // Retrieve information about the module
    printf("module information:\n");
    printf("  type: %s\n", mod.get_metadata("type_long").c_str());
    printf("  title: %s\n", mod.get_metadata("title").c_str());

    // allocate audio buffer
    uint16_t *audio_buffer = new uint16_t[AUDIO_FRAME_SIZE * AUDIO_CHANNELS];
    // play the module
    printf("playing module...\n");

    for (size_t i = 0; i < mod.get_duration_seconds() * AUDIO_SAMPLE_RATE; i += AUDIO_FRAME_SIZE) {
        // vita audio output is S16 (do not use floating points! they will cause static noise.)
        mod.read_interleaved_stereo(AUDIO_SAMPLE_RATE, AUDIO_FRAME_SIZE, reinterpret_cast<int16_t *>(audio_buffer));
        sceAudioOutOutput(audio_out, audio_buffer);
    }
    printf("module play finished!\n");


    // close vitaAudio
    sceAudioOutReleasePort(audio_out);
    // free audio buffer
    delete[] audio_buffer;

    printf("playback done!\n");
    return 0;
}

int main(int argc, char *argv[]) {
    vita2d_init();

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplVita2D_Init();
// Setup style
    ImGui::StyleColorsDark();

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    ImGui_ImplVita2D_TouchUsage(true);
    ImGui_ImplVita2D_UseIndirectFrontTouch(false);
    ImGui_ImplVita2D_UseRearTouch(true);
    ImGui_ImplVita2D_GamepadUsage(true);

    // Main loop
    bool done = false;
    static bool audio_thread_created = false;
    while (!done)
    {
        vita2d_start_drawing();
        vita2d_clear_screen();

        ImGui_ImplVita2D_NewFrame();

        if (ImGui::BeginMainMenuBar()){
            if (ImGui::BeginMenu("Debug")){
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }


        // make new thread for audio playback ONCE
        if (!audio_thread_created) {
            audio_thread_created = true;
            SceUID audio_thread = sceKernelCreateThread("audio_thread", audioThread, 0x10000100, 0x10000, 0, 0, NULL);
            sceKernelStartThread(audio_thread, 0, NULL);
        }




        ImGui::Render();
        ImGui_ImplVita2D_RenderDrawData(ImGui::GetDrawData());

        vita2d_end_drawing();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }



}
