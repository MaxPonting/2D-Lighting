#pragma once

#include <stddef.h>

typedef struct ShaderParser
{
    size_t line_size;
    char* line_buffer;
    size_t shader_size;
    char* vertex_buffer;
    char* fragment_buffer;
} ShaderParser;

int shader_parser_create(ShaderParser* const shader_parser);
void shader_parser_destroy(ShaderParser* const shader_parser);
int shader_parser_parse(ShaderParser* const shader_parser, const char* const file_path);