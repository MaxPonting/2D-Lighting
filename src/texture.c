#include "texture.h"
#include "image.h"

#include <glad/glad.h>

void texture_create(Texture* const texture, const char* const file_path)
{
    stbi_set_flip_vertically_on_load(1);
    stbi_uc* buffer = stbi_load(file_path, &texture->width, &texture->height, &texture->bpp, STBI_rgb_alpha);
    if (buffer == 0)
    {
        printf("Image failed to load\n");
    }

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(buffer);
}

void texture_destroy(Texture* const texture)
{
    glDeleteTextures(GL_TEXTURE_2D, &texture->id);
}