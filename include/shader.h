#pragma once

#include "shader_parser.h"
#include "cglm/struct.h"

void shader_create(unsigned int* const shader_id, ShaderParser* const shader_parser, char* const file_path);
void shader_destroy(const unsigned int shader_id);
void shader_set_uniform_i(const unsigned int shader_id, const char* const name, const int value);
void shader_set_uniform_f(const unsigned int shader_id, const char* const name, const float value);
void shader_set_uniform_3f(const unsigned int shader_id, const char* const name, const vec3 vec);
void shader_set_uniform_matrix_4fv(const unsigned int shader_id, const char* const name, const mat4 matrix);
