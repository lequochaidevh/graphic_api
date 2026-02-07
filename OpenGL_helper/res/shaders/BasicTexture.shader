#shader vertex
#version 330 core
// 3 layers with vertices[]
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP; // model view projection matrix

void main()
{
    gl_Position = u_MVP * vec4(aPosition, 1.0);
    v_TexCoord = aTexCoord;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
    color = texColor * u_Color;
}
