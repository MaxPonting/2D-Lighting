#pragma once

typedef struct Texture
{
    unsigned int id;
    int width, height, bpp;
} Texture;

void texture_create(Texture* const texture, const char* const file_path);
void texture_destroy(Texture* const texture);