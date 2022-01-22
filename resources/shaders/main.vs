#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 fragPos;
uniform mat4 mvp;
uniform mat4 model;

void main(){
    gl_Position = mvp * vec4(aPos, 1.0f);
    //gl_Position = vec4(aPos, 1.0f);
    fragPos = vec3(model * vec4(aPos, 1.0));
}