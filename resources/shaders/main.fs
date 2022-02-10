#version 330 core
in vec3 fragPos;

uniform float farPlane;
uniform samplerCube depthMap[10];

uniform int lightNumber;
uniform vec3 lightPos[10]; 
uniform vec3 lightCol[10];
uniform float specularStr;
uniform vec3 viewPos;

uniform vec3 pallet[4];

out vec4 FragColor;  


float shadowCalculation(int currentLight, vec3 fragmentPos, vec3 lightPos) {
    // get vector between fragment position and light position
    vec3 fragToLight = fragmentPos - lightPos;
    // is the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap[currentLight], fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);  

    //FragColor = vec4(vec3(closestDepth / farPlane), 1.0);
        
    return shadow;
}

vec3 lightCalc(int currentLight, vec3 normal, vec3 lightPosition, vec3 lightColor, float specularStrength, vec3 viewPosition) {
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

    float shadow = shadowCalculation(currentLight, fragPos, lightPosition); 


    //return (ambient + diffuse + specular);
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

void main() {
    //https://stackoverflow.com/questions/14980712/how-to-get-flat-normals-on-a-cube
    vec3 norm = normalize(cross(dFdx(fragPos), dFdy(fragPos)));

    vec3 objectColor = vec3(0.8);

    vec3 lightTotal = vec3(0.0);

    for (int i = 0; i < lightNumber; i++) {
        lightTotal += lightCalc(i, norm, lightPos[i], lightCol[i], specularStr, viewPos);
    }
      

    vec3 result = lightTotal * objectColor;
    //vec3 result = objectColor;

    result.x = pallet[int(round(result.x * 3))].x;
    result.y = pallet[int(round(result.y * 3))].y;
    result.z = pallet[int(round(result.z * 3))].z;



    // int clamp[3];
    // clamp[0] = int(result.x * 4.0);
    // clamp[1] = int(result.y * 4.0);
    // clamp[2] = int(result.z * 4.0);

    // result.x = float(clamp[0]) / 4.0;
    // result.y = float(clamp[1]) / 4.0;
    // result.z = float(clamp[2]) / 4.0;

    FragColor = vec4(result, 1.0); 
}