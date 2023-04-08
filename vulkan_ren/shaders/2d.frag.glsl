#version 450

layout(location = 0) in  vec4 iColour;

layout(location = 0) out vec4 oColour;

void main()
{
  oColour = iColour;
}
