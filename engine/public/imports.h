#pragma once
#include <SDL3/SDL.h>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <filesystem>

/* ENABLE MATH CONSTANTS*/
#define _USE_MATH_DEFINES 1
#include <math.h>

#include <glad.h>

#if __ANDROID__
const std::filesystem::path BASE_PATH = "";
#define ASSETS_PATH std::string("")
#elif __APPLE__
const std::filesystem::path BASE_PATH = SDL_GetBasePath();
#define ASSETS_PATH (BASE_PATH / "assets/").string()
#else
const std::filesystem::path BASE_PATH = SDL_GetBasePath();
#define ASSETS_PATH std::string("assets/")
#endif

#define ENGINE_NAME        "EMBER_ENGINE"
#define ENGINE_VERSION_STR "0.0.1"

