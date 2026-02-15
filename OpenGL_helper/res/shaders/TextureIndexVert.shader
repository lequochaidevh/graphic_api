#version 330 core
// 3 layers with vertices[]
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aTexIndex;

out vec2 v_TexCoord;
out float v_TexIndex;
out vec3 aColorVert;
uniform mat4 u_MVP; // model view projection matrix
out vec4 vtx_uniformNoUsed;

void main()
{
    gl_Position = u_MVP * vec4(aPosition, 1.0);
    v_TexCoord = aTexCoord;
	aColorVert = aColor;
    v_TexIndex = aTexIndex;
	vtx_uniformNoUsed = vec4(1.0);
}