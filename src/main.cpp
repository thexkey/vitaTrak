/*
 * main.cpp
 * -------------
 * Purpose: Main entry point for the vitaTrak application.
 * Notes  : This is really rushed, but it works (barely).
 * TODO: fix some modules crashing the application when playing for a while
 *          (e.g. "coretex - home.xm", "dreamlan.mod", "radix_-_images.xm",
 *          "abstract_illusions.xm", "a-aafter.xm", "club_bizzare_95_remix.xm",
 *          probably many, many more?)
 *          you can find these on ModArchive for bug replication.
 */


#include <new>
#include <imgui_vita2d/imgui_vita.h>
#include <cstdio>
#include <vita2d.h>
#include <config.h>
#include <dirent.h>
#include <psp2/kernel/threadmgr.h>
#include <openmpt/playback.h>
#include "main.h"

#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_CHANNELS 64
#define AUDIO_FRAME_SIZE 2048

const char *sort_modes_local_str[] = {
        "Alphabetical (A-Z)",
        "Alphabetical (Z-A)",
        "By File Size (Smallest)",
        "By File Size (Largest)",
        "By Module Length (Shortest)",
        "By Module Length (Longest)"
};

#if VITATRAK_MODARCHIVE_SUPPORT == 1
const char *sort_modes_modarchive_srt[] ={
        "Alphabetical (A-Z)",
        "Alphabetical (Z-A)",
        "By Rating (Lowest)",
        "By Rating (Highest)",
#if VITATRAK_MODARCHIVE_API_LEVEL == 5
        "Featured",
        "Top Downloads",
        "Top Artists",
        "Top Favorite Modules",
        "New Additions",
#endif
};
#endif

bool isInitStatusStringSet = false;
int main(int argc, char *argv[]) {
    std::string playback_status;
    if(!isInitStatusStringSet){
        playback_status = "doing literally nothing_init";
        isInitStatusStringSet = true;
    }

    sceIoMkdir("ux0:data/vitaTrak", 0777);
#if VITATRAK_MODARCHIVE_SUPPORT == 1
    sceIoMkdir("ux0:data/vitaTrak/downloads", 0777);
#endif

    // Initialize vita2d
    vita2d_init();

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplVita2D_Init();

    // Setup style
    ImGui::StyleColorsDark();

    ImGui_ImplVita2D_GamepadUsage(true);
    ImGui_ImplVita2D_MouseStickUsage(false);
    ImGui_ImplVita2D_TouchUsage(false);
    ImGui::GetIO().MouseDrawCursor = false;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    // Main loop
    while(true) {
        vita2d_start_drawing();
        vita2d_clear_screen();

        ImGui_ImplVita2D_NewFrame();
        //ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            char title[256];
            sprintf(title, "vitaTrak - Currently %s", playback_status.c_str());
            ImGui::Text(title);
            ImGui::EndMainMenuBar();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        ImGui::PopStyleVar(2);

        // disable alpha transparency on window
        ImGui::SetNextWindowBgAlpha(1.0f);

        ImGui::SetNextWindowPos(ImVec2(0, 21));
        ImGui::SetNextWindowSize(ImVec2(553, 523));
        ImGui::Begin("File Browser", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoSavedSettings);

        // light-cyan bg color
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.8f, 1.0f, 1.0f, 1.0f));

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Search: ");
        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
        if (ImGui::Button("what", ImVec2(-1.0f, 0.0f))) {
            // dummy func for now
        }

        // scan ux0:/data/vitaTrak for music modules and add em as buttons
        std::string path = "ux0:data/vitaTrak";
        DIR *dir;
        struct dirent *ent;
        try{
            if ((dir = opendir (path.c_str())) != nullptr ) {
                /* print all the files and directories within directory */
                while ((ent = readdir (dir)) != nullptr) {
                   if (ent->d_name[0] != '.') {
                        ImGui::AlignTextToFramePadding();
                        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
                        if (ImGui::Button(ent->d_name, ImVec2(-1.0f, 0.0f))) {

                            // prepare music thread
                            SceUID audio_thread = sceKernelCreateThread("audio_thread", &musicPlaybackThread, 0x10000100, 0x10000, 0, 0, nullptr);

                            // call new thread to play music with an argument containing the file path
                            // craft data for thread calling (file path as arg, then calculate that in bytes)
                            std::string data = path + "/" + ent->d_name;
                            printf("data: %s\n", data.c_str());
                            sceKernelStartThread(audio_thread, data.length()+1, static_cast<void*>(const_cast<char*>(data.c_str())));
                        }
                    }
                }
                closedir (dir);
            }
        }
        catch (const std::exception& e) {
            printf("exception caught: %s\n", e.what());
            return EXIT_FAILURE;
        }

        // exit button in red (even when hovered)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::AlignTextToFramePadding();
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
        if (ImGui::Button("Exit", ImVec2(-1.0f, 0.0f))) {
            break;
        }
        ImGui::PopStyleColor(2);



        ImGui::End(); // end file browser window


        ImGui::Render();
        ImGui_ImplVita2D_RenderDrawData(ImGui::GetDrawData());

        vita2d_end_drawing();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }

    // Cleanup
    ImGui_ImplVita2D_Shutdown();
    ImGui::DestroyContext();
    vita2d_fini();

    return 0;
}
