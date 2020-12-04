#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
layout (location = 1) in vec2 aTexCoord;

uniform float horizonOffset;//!!!:use float instead of vec1 here. Maybe no vec1 definition in the openGL.
//out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos.x+horizonOffset,aPos.y,aPos.z, 1.0);
	//ourColor = aColor;
	TexCoord = aTexCoord;
};