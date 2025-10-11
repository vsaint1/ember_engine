#include "core/engine.h"
#include <SDL3/SDL_main.h>


int main(int argc, char* argv[]) {
    if (!GEngine->initialize(1280, 720, "Ember Engine - Client")) {
        return -1;
    }


    Cube cube;
    cube.color = glm::vec3(0.2f, 0.7f, 0.3f);
    cube.size  = glm::vec3(2.0f);
    Camera3D camera;

    camera.position = glm::vec3(0, 1.5f, 10.0f);
    Transform3D cube_transform;
    while (GEngine->is_running) {
        GEngine->get_timer().tick();


        while (SDL_PollEvent(&GEngine->event)) {

            if (GEngine->event.type == SDL_EVENT_QUIT) {
                GEngine->is_running = false;
            }


            if (GEngine->event.type == SDL_EVENT_KEY_DOWN) {

                if (GEngine->event.key.scancode == SDL_SCANCODE_F9) {
                    GEngine->get_config().is_debug = !GEngine->get_config().is_debug;
                }
            }


            if (GEngine->event.type == SDL_EVENT_WINDOW_RESIZED) {
                int new_w      = GEngine->event.window.data1;
                int new_h      = GEngine->event.window.data2;
                auto& app_win  = GEngine->get_config().get_window();
                app_win.width  = new_w;
                app_win.height = new_h;
            }

        }


        auto view = camera.get_view();
        auto proj = camera.get_projection(GEngine->get_config().get_window().width, GEngine->get_config().get_window().height);
        GEngine->get_renderer()->clear(GEngine->get_config().get_environment().clear_color);

        GEngine->get_renderer()->draw_cube(cube_transform, cube);

        GEngine->get_renderer()->flush(view, proj);

        GEngine->get_renderer()->present();


        SDL_Delay(16); // ~60 FPS
    }

    return 0;
}
