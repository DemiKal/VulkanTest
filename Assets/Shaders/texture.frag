#version 450

layout (binding = 1) uniform sampler2D texture1;

layout(location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColor;

void main()
{
	vec4 color = texture(texture1, inTexCoord);

    outFragColor = color;
}