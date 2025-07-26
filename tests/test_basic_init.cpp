#include "core/renderer/ember_gl.h"
#include <SDL3/SDL_main.h>


int SCREEN_WIDTH  = 1280;
int SCREEN_HEIGHT = 720;


SDL_AppResult SDL_AppInit(void** app_state, int argc, char** argv) {

    if (!InitWindow("Window sample", SCREEN_WIDTH, SCREEN_HEIGHT, RendererType::OPENGL, SDL_WINDOW_RESIZABLE)) {
        return SDL_APP_FAILURE;
    }

    if (!InitAudio()) {
        return SDL_APP_FAILURE;
    }

    LOG_INFO("Device Name %s", SystemInfo::GetDeviceName().c_str());
    LOG_INFO("Device Model %s", SystemInfo::GetDeviceModel().c_str());
    LOG_INFO("Device UniqueIdentifier %s", SystemInfo::GetDeviceUniqueIdentifier().c_str());


    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void* app_state) {

    core.Time->Update();

    auto pKey = SDL_GetKeyboardState(0);

    ClearBackground({120, 100, 100, 255});

    BeginDrawing();


    EndDrawing();

    core.Time->FixedFrameRate(60);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* app_state, SDL_Event* event) {


    auto pKey = SDL_GetKeyboardState(0);

    if (pKey[SDL_SCANCODE_ESCAPE] || event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }


    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* app_state, SDL_AppResult result) {

    CloseAudio();

    CloseWindow();
}
