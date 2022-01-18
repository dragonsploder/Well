#version 330 core
in vec3 pos;
out vec4 FragColor;  
  
void main() {
    vec3 point = vec3(2.0, 1.0, 1.0);
    //FragColor = vec4((pos + 1)/2, 1.0);
    FragColor = vec4(distance(pos, point)/5, distance(pos, point)/5, distance(pos, point)/5, 1.0);
}