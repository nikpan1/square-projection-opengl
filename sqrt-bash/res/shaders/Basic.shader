#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
uniform mat4 mtx;

out float v_clr;
void main()
{
	gl_Position = vec4(position[0], position[1], position[2], 1) * mtx;
	v_clr = position[3];
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;
uniform vec4 u_Color;
in float v_clr;
void main()
{
	color = vec4(u_Color[0], u_Color[1], v_clr, u_Color[3]);
}
