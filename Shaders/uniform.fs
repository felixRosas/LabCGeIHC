#version 330 core
in vec4 ourColor;
out vec4 color;
//variable global de globalColor
uniform vec4 globalColor;
void main()
{
	color = globalColor * ourColor;
}