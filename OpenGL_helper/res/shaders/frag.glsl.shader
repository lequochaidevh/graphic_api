#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 aColorVert;
uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform float u_Time;
out vec4 frag_uniformNoUsed;

void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
	// color = texColor * vec4(aColorVert, 1.0);
    vec4 result = vec4(v_TexCoord, u_Time, 1.0)
              * texColor
              * u_Color
              * vec4(aColorVert, 1.0) 
              * 2.0;  // boost
    color = clamp(result, 0.0, 1.0);
    frag_uniformNoUsed = texColor * u_Color;;
}