#include "shader.h"

#include <glad/glad.h>

#include <stdio.h>
#include <malloc.h>

static unsigned int compile_shader(unsigned int type, const char* const source);

void shader_create(unsigned int* const shader_id, ShaderParser* const shader_parser, char* const file_path)
{
    shader_parser_parse(shader_parser, file_path);

    unsigned int program_id = glCreateProgram();
    unsigned int vertex_id = compile_shader(GL_VERTEX_SHADER, shader_parser->vertex_buffer);
    unsigned int fragment_id = compile_shader(GL_FRAGMENT_SHADER, shader_parser->fragment_buffer);

    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);
    glValidateProgram(program_id);

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);

    *shader_id = program_id;
}

void shader_destroy(const unsigned int shader_id)
{
    glDeleteProgram(shader_id);
}

static unsigned int compile_shader(unsigned int type, const char* const source) 
{
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, 0);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        printf("Shader compilation failed:%s\n", message);
        glDeleteShader(id);
        return 0;
    }
 
    return id;
}

void shader_set_uniform_i(const unsigned int shader_id, const char* const name, const int value)
{
    glUniform1i(glGetUniformLocation(shader_id, name), value);
}

void shader_set_uniform_f(const unsigned int shader_id, const char* const name, const float value)
{
    glUniform1f(glGetUniformLocation(shader_id, name), value);
}

void shader_set_uniform_3f(const unsigned int shader_id, const char* const name, const vec3 vec)
{
    glUniform3f(glGetUniformLocation(shader_id, name), vec[0], vec[1], vec[2]);
}

void shader_set_uniform_matrix_4fv(const unsigned int shader_id, const char* const name, const mat4 matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(shader_id, name), 1, 0, (float*)matrix);
}