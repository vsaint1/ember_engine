﻿#include "core/renderer/opengl/ember_gl.h"
#include <SDL3/SDL_main.h>


int SCREEN_WIDTH  = 1280;
int SCREEN_HEIGHT = 720;

Font mine_font;

Texture player_texture,p2_texture;
bool bShowMetrics = false;

Color background_color = {120, 100, 100, 255};

Audio *mine_music, *tel_music, *random_music;
Camera2D camera = Camera2D(480, 270);

SDL_AppResult SDL_AppInit(void** app_state, int argc, char** argv) {


    if (!GEngine->Initialize("Example - new API", SCREEN_WIDTH, SCREEN_HEIGHT, RendererType::OPENGL,
                             SDL_WINDOW_RESIZABLE)) {
        return SDL_APP_FAILURE;
    }


    camera.transform.position = glm::vec3(0.f, 0.f, 0.0f);
    camera.transform.rotation = glm::vec3(0.f);

    // assets in examples/assets
    mine_font = GEngine->GetRenderer()->LoadFont("fonts/Minecraft.ttf", 32);

    player_texture = GEngine->GetRenderer()->LoadTexture("sprites/Character_001.png");
    p2_texture = GEngine->GetRenderer()->LoadTexture("sprites/Character_002.png");
    mine_music   = Audio::Load("sounds/lullaby.mp3");
    tel_music    = Audio::Load("sounds/the_entertainer.ogg");
    random_music = Audio::Load("sounds/test.flac");

    mine_music->Play();

    LOG_INFO("Device Name %s", SystemInfo::GetDeviceName().c_str());
    LOG_INFO("Device Model %s", SystemInfo::GetDeviceModel().c_str());
    LOG_INFO("Device UniqueIdentifier %s", SystemInfo::GetDeviceUniqueIdentifier().c_str());

    return SDL_APP_CONTINUE;
}

const char* gui_text = R"(
Lorem Ipsum is simply dummy text of the printing and typesetting industry.
Lorem Ipsum has been the industry's standard dummy text ever since the 1500s,
when an unknown printer took a galley of type and scrambled it to make a type specimen book.
It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged.
It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages,
and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.

)";

Color text_color     = {255, 255, 255, 255};

int entities = 0;

SDL_AppResult SDL_AppIterate(void* app_state) {

    GEngine->GetInputManager()->Update();

    GEngine->GetTimeManager()->Update();

    GEngine->GetRenderer()->ClearBackground(background_color);
    GEngine->GetRenderer()->BeginDrawing();

    static float angle         = 0.0f;
    static glm::vec3 position = {200, 300, 0};

    static Transform2D transform = {
        glm::vec3(300.f, 100.f, 0.f),
        glm::vec2(1.f),
        0.0f
    };


    static Transform2D transform2;
    transform2.Position = {500, 100, 0.001f};
    transform2.Rotation = 0.0f;
    transform2.Scale    = {1, 1};

    static Transform2D transform3 = {
        glm::vec3(300.f, 450.f, 0.f),
        glm::vec2(1.f),
        0.0f
    };


    for (int i = 0; i < entities; i++) {
        GEngine->GetRenderer()->DrawTextureEx(player_texture, {0, 0, 32, 32}, {i * 64, i * 64, 64, 64}, glm::vec2(0.5f, 0.5f), angle,
                                              0.2f);
    }

    GEngine->GetRenderer()->DrawLine({200, 200, 0}, {300, 300, 0}, {0, 255, 0, 255}, 2.f);

    GEngine->GetRenderer()->DrawTexture(player_texture, transform2,{0,0} );

    transform2.Position.z = 0.002f;

    GEngine->GetRenderer()->DrawRect(transform2, {512, 512}, {255, 255, 255, 255}, 2.f);

    GEngine->GetRenderer()->DrawCircle({300, 650,0.1}, 40, {255, 255, 255, 255});
    GEngine->GetRenderer()->DrawCircleFilled({300, 300,0.2}, 20, {255,0 , 255, 255});
    GEngine->GetRenderer()->DrawTriangle({100, 100,0}, {200, 100,0}, {150, 200,0}, {255, 255, 0, 255});

    GEngine->GetRenderer()->DrawTextureEx(p2_texture, {0, 0, 32, 32}, {100, 200, 128, 128}, glm::vec2(0.5f, 0.5f),
                                          angle);

    GEngine->GetRenderer()->DrawText(mine_font, "I think this works\n No internationalization =(", transform,
                                     {255, 255, 255, 255}, 20.f);

    GEngine->GetRenderer()->DrawText(mine_font, "Hello World!", transform3,
                                     {255, 255, 255, 255}, 20.f,
                                     {.Outline = {
                                          .bEnabled  = true,
                                          .color     = Color(255, 0, 0, 255).GetNormalizedColor(),
                                          .thickness = 0.35f,
                                      }});


    ImGui::SetNextWindowSize(ImVec2(350.f, 600.f), ImGuiCond_FirstUseEver);
    ImGui::Begin("[DEMO] - new API", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::InputInt("Create Entity", &entities, 10);

    float temp_color[4] = {text_color.r / 255.0f, text_color.g / 255.0f, text_color.b / 255.0f, text_color.a / 255.0f};


    ImGui::Text("Player");
    ImGui::DragFloat3("Position##player", &position.x, 1);
    ImGui::SliderFloat("Angle##player", &angle, 0.0f, 360.0f, "%.2f");

    ImGui::Text("Text");
    ImGui::SliderFloat3("Position##text", &transform2.Position.x, 0.0f, GEngine->Window.width);
    ImGui::SliderFloat3("Scale##text", &transform2.Scale.x, 0.0f, 10.0f);
    ImGui::SliderFloat3("Rotation##text", &transform2.Rotation, 0.0f, 360.0f);


    if (ImGui::ColorEdit4("Text color", temp_color), ImGuiColorEditFlags_NoInputs) {
        text_color.r = static_cast<uint8_t>(temp_color[0] * 255);
        text_color.g = static_cast<uint8_t>(temp_color[1] * 255);
        text_color.b = static_cast<uint8_t>(temp_color[2] * 255);
        text_color.a = static_cast<uint8_t>(temp_color[3] * 255);
    }


    ImGui::Text("Camera");
    ImGui::DragFloat3("Position##Camera", &camera.transform.position.x, 1.0f);
    ImGui::DragFloat("Zoom##Camera", &camera.zoom, 0.1f, 0.01f, 10.0f);


    ImGui::Text("Musics");

    if (!mine_music->IsPlaying()) {

        if (ImGui::Button("Play Lullaby")) {
            mine_music->Play();
        }

    } else {
        if (ImGui::Button("Stop Lullaby")) {
            mine_music->Pause();
        }
    }

    if (ImGui::SliderFloat("Volume##1", &mine_music->volume, 0.0f, 1.0f)) {
        mine_music->SetVolume(mine_music->volume);
    }

    if (!tel_music->IsPlaying()) {

        if (ImGui::Button("Play The Entertainer")) {
            tel_music->Play();
        }

    } else {
        if (ImGui::Button("Stop The Entertainer")) {

            tel_music->Pause();
        }
    }

    if (ImGui::SliderFloat("Volume##2", &tel_music->volume, 0.0f, 1.0f)) {
        tel_music->SetVolume(tel_music->volume);
    }

    if (!random_music->IsPlaying()) {

        if (ImGui::Button("Play Unknown")) {
            random_music->Play();
        }

    } else {
        if (ImGui::Button("Stop The Unknown")) {

            random_music->Pause();
        }
    }

    if (ImGui::SliderFloat("Volume##3", &random_music->volume, 0.0f, 1.0f)) {
        random_music->SetVolume(random_music->volume);
    }


    ImGui::Text("Engine");
    if (ImGui::SliderFloat("Musics Volume", &GEngine->Audio.global_volume, 0.0f, 1.0f)) {
        Audio_SetMasterVolume(GEngine->Audio.global_volume);
    }

    ImGui::Checkbox("Metrics", &bShowMetrics);

    ImGui::End();


    if (bShowMetrics) {
        ImGui::Begin("Metrics", &bShowMetrics);

        const ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Application average: %2.03f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Delta Time: %f", GEngine->GetTimeManager()->GetDeltaTime());
        ImGui::Text("Elapsed Time: %.3f", GEngine->GetTimeManager()->GetElapsedTime());
        ImGui::Text("Frame count: %llu", GEngine->GetTimeManager()->GetFrameCount());

        ImGui::Text("RAM Usage: %d MB", GetMemoryUsage());


        if (ImGui::Button("Close Me")) {
            bShowMetrics = false;
        }

        ImGui::End();
    }

    GEngine->GetRenderer()->EndDrawing();

    GEngine->GetTimeManager()->FixedFrameRate();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* app_state, SDL_Event* event) {

    GEngine->GetInputManager()->ProcessEvents(event);

    auto pKey = SDL_GetKeyboardState(nullptr);

    if (pKey[SDL_SCANCODE_ESCAPE] || event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }


    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        GEngine->ResizeWindow(event->window.data1, event->window.data2);

        GEngine->GetRenderer()->Resize(event->window.data1, event->window.data2);
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* app_state, SDL_AppResult result) {

    GEngine->GetRenderer()->UnloadFont(mine_font);

    GEngine->GetRenderer()->UnloadTexture(player_texture);

    GEngine->Shutdown();
}

