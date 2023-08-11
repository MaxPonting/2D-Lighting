#include "shader_parser.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

int shader_parser_create(ShaderParser* const shader_parser)
{   
    if (!shader_parser)
    {
        printf("Cannot create a null shader parser\n");
        return 1;
    }

    shader_parser->line_size = 100;
    shader_parser->line_buffer = malloc(sizeof(char) * shader_parser->line_size);
    shader_parser->shader_size = 2000;
    shader_parser->vertex_buffer = malloc(sizeof(char) * shader_parser->shader_size);
    shader_parser->fragment_buffer = malloc(sizeof(char) * shader_parser->shader_size);

    return 0;
}

void shader_parser_destroy(ShaderParser* const shader_parser)
{
    free(shader_parser->line_buffer);
    free(shader_parser->vertex_buffer);
    free(shader_parser->fragment_buffer);
}

int shader_parser_parse(ShaderParser* const shader_parser, const char* const file_path)
{
    FILE* file_stream;
    file_stream = fopen(file_path, "r");

    if(file_stream == 0)
    {
        printf("file failed to open\n");
        return 0;
    }
    
    const char* vertex_sign = "#shader vertex\n";
    const char* fragment_sign = "#shader fragment\n";
    
    int shader_type = -1;
    size_t vertex_count = 0;
    size_t fragment_count = 0;
    while (fgets(shader_parser->line_buffer, shader_parser->line_size, file_stream) != 0)
    {
        if(strcmp(shader_parser->line_buffer, vertex_sign) == 0)      
        {
            shader_type = 0;   
            continue;  
        }                       
        else if(strcmp(shader_parser->line_buffer, fragment_sign) == 0)
        {
            shader_type = 1; 
            continue;
        }       
                               
        if(shader_type == 0)
        {
            int i = 0;
            while(1)
            {
                if (shader_parser->line_buffer[i] == '\0')
                {   
                    break;
                }
                
                shader_parser->vertex_buffer[vertex_count] = shader_parser->line_buffer[i];
                i++;
                vertex_count++;
            }
        }
        else if(shader_type == 1)
        {
            int i = 0;
            while(1)
            {
                if (shader_parser->line_buffer[i] == '\0')
                {   
                    break;
                }
                                    
                shader_parser->fragment_buffer[fragment_count] = shader_parser->line_buffer[i];
                i++;
                fragment_count++;
            }
        }

        if (vertex_count == shader_parser->shader_size || fragment_count == shader_parser->shader_size)
        {
            printf("Shader read failed: Buffer capacity reached\n");
            return 0;
        }
    }

    shader_parser->vertex_buffer[vertex_count] = '\0';
    shader_parser->fragment_buffer[fragment_count] = '\0';

    //printf("Vertex Shader\n%s", shader_parser->vertex_buffer);
    //printf("Fragment Shader\n%s", shader_parser->fragment_buffer);

    fclose(file_stream);

    return 1;
}