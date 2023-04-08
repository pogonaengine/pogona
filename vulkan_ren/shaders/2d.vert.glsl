#version 450

layout(location = 0) in  vec3 iPosition;
layout(location = 1) in  vec4 iColour;

layout(location = 0) out vec4 oColour;

void main()
{
	gl_Position = vec4(iPosition, 1.0f);
  oColour = iColour;
}
