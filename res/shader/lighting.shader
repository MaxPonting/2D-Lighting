#shader vertex
#version 330 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TextureCoordinates;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 v_TextureCoordinates;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * a_Position;
    v_TextureCoordinates = a_TextureCoordinates;
}

#shader fragment
#version 330 core

in vec2 v_TextureCoordinates;

uniform sampler2D u_Textures[3];
uniform float u_Ambient;
uniform vec3 u_LightPos[2];
uniform float u_LightRadius[2];

out vec4 o_Color;

void main()
{     
    vec4 color = texture2D(u_Textures[0], v_TextureCoordinates);
    vec3 normal = texture2D(u_Textures[1], v_TextureCoordinates).xyz;
    normal = normalize(normal * 2.0 - 1.0);
    float specular = texture2D(u_Textures[2], v_TextureCoordinates).x;

    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

    for (int i = 0; i < 2; i++)
    {
        vec3 toLight = u_LightPos[i] - gl_FragCoord.xyz;
        vec3 lightDirection = normalize(toLight);

        float brightness = clamp(1.0 - (length(toLight) / u_LightRadius[i]), 0.0, 1.0);

        float diff = max(dot(lightDirection, normal), 0.0);
        vec4 diffuse = diff * color;

        o_Color = max(brightness * diffuse * specular * lightColor * 3 + u_Ambient * color, o_Color);
    }
}