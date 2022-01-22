#version 330 core
in vec3 fragPos;

uniform int lightNumber;
uniform vec3 lightPos[10]; 
uniform vec3 lightCol[10];
uniform float specularStr[10];
uniform vec3 viewPos;

out vec4 FragColor;  

vec3 lightCalc(vec3 normal, vec3 lightPosition, vec3 lightColor, float specularStrength, vec3 viewPosition) {
    //https://learnopengl.com/Lighting/Basic-Lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 lightDir = normalize(lightPosition - fragPos);  
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;


    vec3 viewDir = normalize(viewPosition - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    //return (ambient + diffuse + specular);
    return (ambient + diffuse + specular);
}
  
void main() {
    //https://stackoverflow.com/questions/14980712/how-to-get-flat-normals-on-a-cube
    vec3 norm = normalize(cross(dFdx(fragPos), dFdy(fragPos)));

    vec3 objectColor = vec3(0.8);

    vec3 lightTotal = vec3(0.0);

    for (int i = 0; i < lightNumber; i++) {
        lightTotal += lightCalc(norm, lightPos[i], lightCol[i], specularStr[i], viewPos);
    }
      

    vec3 result = lightTotal * objectColor;
    //vec3 result = objectColor;
    FragColor = vec4(result, 1.0);  
}