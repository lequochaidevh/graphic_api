#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 aColorVert;
in float v_TexIndex;
uniform vec4 u_Color;
uniform sampler2D u_Textures[2];

out vec4 frag_uniformNoUsed;

void main()
{
    int index = int(v_TexIndex);
    vec4 texColor = texture(u_Textures[index], v_TexCoord);
    color = texColor * vec4(aColorVert, 1.0);
    frag_uniformNoUsed = texColor * u_Color;
}