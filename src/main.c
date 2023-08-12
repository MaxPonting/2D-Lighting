#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <cglm/struct.h>

#include <stdio.h>

#include "shader.h"
#include "shader_parser.h"
#include "texture.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

int main()
{   
    const float window_width = 1280;
    const float window_height = 720;

    // Window Creation

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "2D Lighting", 0, 0);
    if (window == 0)
    {
        printf("[ERROR][GLFW][Failed to create window]\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    // Load OpenGL Functions

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("[ERROR][GLAD][Failed to initialize]\n");
        return -1;
    }

    // OpenGL Setup

    glViewport(0, 0, window_width, window_height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Data

    float vertices[] = {
       -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
       -0.5f,  0.5f, 0.0f, 1.0f,
        0.5f,  0.5f, 1.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3
    };

    // Buffers

    unsigned int vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    unsigned int vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int index_buffer_id;
    glGenBuffers(1, &index_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    // Shader

    ShaderParser shader_parser;
    shader_parser_create(&shader_parser);
    
    unsigned int shader_program_id;
    shader_create(&shader_program_id, &shader_parser, "../../res/shader/lighting.shader");

    // Textures

    Texture color_map;
    texture_create(&color_map, "../../res/texture/circle_color_map.png");

    Texture normal_map;
    texture_create(&normal_map, "../../res/texture/circle_normal_map.png");

    Texture specular_map;
    texture_create(&specular_map, "../../res/texture/circle_specular_map.png");

    // Ambient

    float ambient = 0.2f;

    // Lights

    vec3 light_positions[2] = {{ window_width / 2, window_height / 2, 100}, { window_width / 2 - 160, window_height / 2, 100 }};
    float light_radii[2] = { 1000, 200 };
    vec4 light_colours[2] = {{1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}};

    // Camera

    vec2 camera_position = {0, 0};

    // Nuklear

    struct nk_glfw glfw = {0};
    struct nk_context *ctx;

    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);

    {
        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&glfw, &atlas);
        nk_glfw3_font_stash_end(&glfw);
    }

    // Render Loop

    while(!glfwWindowShouldClose(window))
    {   
        // Input
        glfwPollEvents();

        double x_pos, y_pos;
        glfwGetCursorPos(window, &x_pos, &y_pos);

        if(ambient < 0)
            ambient = 0;
        else if(ambient > 1)
            ambient = 1;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera_position[1] += 1;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera_position[1] -= 1;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera_position[0] += 1;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera_position[0] -= 1;

        // Matrix

        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glm_scale(model, (vec3){color_map.width * 2, color_map.height * 2, 1});
        
        mat4 view = GLM_MAT4_IDENTITY_INIT;
        glm_translate(view, (vec3){-camera_position[0], -camera_position[1], 0});

        mat4 projection = GLM_MAT4_IDENTITY_INIT;
        glm_ortho(-window_width / 2, window_width / 2, -window_height / 2, window_height / 2, -1, 1, projection);

        // Light Transformations

        vec3 light_positions_transformed[2];

        for (int i = 0; i < 2; i++)
        {   
            light_positions_transformed[i][0] = light_positions[i][0];
            light_positions_transformed[i][1] = light_positions[i][1];
            light_positions_transformed[i][2] = light_positions[i][2];
            light_positions_transformed[i][0] -= camera_position[0];
            light_positions_transformed[i][1] -= camera_position[1];
        }

        light_positions_transformed[0][0] = x_pos;
        light_positions_transformed[0][1] = window_height - y_pos;

        // Render

        glViewport(0, 0, window_width, window_height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program_id);

        shader_set_uniform_matrix_4fv(shader_program_id, "u_View", view);
        shader_set_uniform_matrix_4fv(shader_program_id, "u_Projection", projection);
        shader_set_uniform_matrix_4fv(shader_program_id, "u_Model", model);

        glUniform1iv(glGetUniformLocation(shader_program_id, "u_Textures"), 3, (int[]){0, 1, 2});
        glUniform1f(glGetUniformLocation(shader_program_id, "u_Ambient"), ambient);
        glUniform3fv(glGetUniformLocation(shader_program_id, "u_LightPos"), 2, light_positions_transformed);
        glUniform1fv(glGetUniformLocation(shader_program_id, "u_LightRadius"), 2, light_radii);
        glUniform4fv(glGetUniformLocation(shader_program_id, "u_LightColors"), 2, light_colours);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, color_map.id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normal_map.id);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, specular_map.id);

        glBindVertexArray(vertex_array_id);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(0);

        // Nuklear

        nk_glfw3_new_frame(&glfw);
        if (nk_begin(ctx, "Debug", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            struct nk_colorf color;
            color.r = light_colours[0][0];
            color.g = light_colours[0][1];
            color.b = light_colours[0][2];
            color.a = light_colours[0][3];

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Light Colour", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(color), nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                color = nk_color_picker(ctx, color, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                color.r = nk_propertyf(ctx, "#R:", 0, color.r, 1.0f, 0.01f, 0.005f);
                color.g = nk_propertyf(ctx, "#G:", 0, color.g, 1.0f, 0.01f, 0.005f);
                color.b = nk_propertyf(ctx, "#B:", 0, color.b, 1.0f, 0.01f, 0.005f);
                color.a = nk_propertyf(ctx, "#A:", 0, color.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(ctx);
            }

            nk_layout_row_dynamic(ctx, 30, 1);
            nk_property_float(ctx, "2nd Light Radius", 0, &light_radii[1], 600, 20, 1);

            for (int i = 0; i < 2; i++)
            {
                light_colours[i][0] = color.r;
                light_colours[i][1] = color.g;
                light_colours[i][2] = color.b;
                light_colours[i][3] = color.a;
            }
        }
        nk_end(ctx);

 
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

        // Swap Buffers

        glfwSwapBuffers(window);
    }

    // Finish

    glDeleteProgram(shader_program_id);
    glDeleteVertexArrays(1, &vertex_array_id);
    glDeleteBuffers(1, &vertex_buffer_id);
    glDeleteBuffers(1, &index_buffer_id);

    nk_glfw3_shutdown(&glfw);
    glfwTerminate();

    return 0;
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    printf("[ERROR][OPENGL][%s]\n", message);
}

