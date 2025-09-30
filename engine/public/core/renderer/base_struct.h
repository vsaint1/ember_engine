#pragma once

#include "stdafx.h"

enum class CUBEMAP_ORIENTATION {
    DEFAULT, // original layout
    TOP, //  (+Y)
    BOTTOM, // (-Y)
    FLIP_X, // flip left-right
    FLIP_Y // flip up-down
};

struct Tokens {
    std::string text;
    bool is_emoji;
};


class Font {
public:
    virtual ~Font() = default;

    virtual glm::vec2 get_size(const std::string& text) {
        return {0, 0};
    }
};


class Texture {
public:
    Uint32 id        = 0;
    int width        = 0;
    int height       = 0;
    std::string path = "";

    virtual ~Texture() = default;
};
