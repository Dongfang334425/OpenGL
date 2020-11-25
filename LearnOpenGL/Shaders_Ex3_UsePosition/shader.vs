#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform float horizonOffset;//!!!:use float instead of vec1 here. Maybe no vec1 definition in the openGL.
out vec3 ourPosition;
void main()
{
	gl_Position = vec4(aPos.x+horizonOffset,aPos.y,aPos.z, 1.0);
	ourPosition = aPos;
};