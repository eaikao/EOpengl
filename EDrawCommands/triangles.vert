#version 430 core
uniform mat4 g_modelMatrixLoc;
uniform mat4 g_projectionMatrixLoc;

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

out vec4 vs_fs_color;

void main(void)
{
    vs_fs_color = color;
    gl_Position = g_projectionMatrixLoc * g_modelMatrixLoc * position;
}