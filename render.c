#include "well.h"

void initRender(struct Render* render, char vertexPath[500], char fragmentPath[500]) {
    DEBUG_LOG("Init render");

    char *vertexShaderSource;
    size_t vertexShaderSourceSize;
    DEBUG_LOG("Reading vertex shader");
    readFile(vertexPath, &vertexShaderSource, &vertexShaderSourceSize);
    const char* vertexShaderSourceConst = vertexShaderSource;

    char *fragmentShaderSource;
    size_t fragmentShaderSourceSize;
    DEBUG_LOG("Reading fragment shader");
    readFile(fragmentPath, &fragmentShaderSource, &fragmentShaderSourceSize);
    const char* fragmentShaderSourceConst = fragmentShaderSource;

    // build and compile our shader program
    // vertex shader
    DEBUG_LOG("Compiling vertex shader");
    GL_CALL(unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER));
    GL_CALL(glShaderSource(vertexShader, 1, &vertexShaderSourceConst, NULL));
    GL_CALL(glCompileShader(vertexShader));
    // check for shader compile errors
    int success;
    char infoLog[512];
    GL_CALL(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));
    if (!success) {
        GL_CALL(glGetShaderInfoLog(vertexShader, 512, NULL, infoLog));
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
        printf("%s\n", infoLog);
    }

    // fragment shader
    DEBUG_LOG("Reading fragment shader");
    GL_CALL(unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER));
    GL_CALL(glShaderSource(fragmentShader, 1, &fragmentShaderSourceConst, NULL));
    GL_CALL(glCompileShader(fragmentShader));
    // check for shader compile errors
    GL_CALL(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success));
    if (!success) {
        GL_CALL(glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog));
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
        printf("%s\n", infoLog);
    }
    // link shaders
    DEBUG_LOG("Linking shaders to program");
    GL_CALL(render->program = glCreateProgram());
    GL_CALL(glAttachShader(render->program, vertexShader));
    GL_CALL(glAttachShader(render->program, fragmentShader));
    GL_CALL(glLinkProgram(render->program));
    // check for linking errors
    GL_CALL(glGetProgramiv(render->program, GL_LINK_STATUS, &success));
    if (!success) {
        GL_CALL(glGetShaderInfoLog(render->program, 512, NULL, infoLog));
        printf("ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n");
        printf("%s\n", infoLog);
    }
    DEBUG_LOG("Deleting shaders");
    GL_CALL(glDeleteShader(vertexShader));
    GL_CALL(glDeleteShader(fragmentShader));
}


void freeRender(struct Render* render) {
    DEBUG_LOG("Free renderer");
    GL_CALL(glDeleteProgram(render->program));
}


void renderFrame(struct Render* render, struct Camera* camera, int modelSize, struct Model models[], int lightSize, struct Light light[]) {
    DEBUG_LOG("Render Frame\n");
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    DEBUG_LOG("    Recalculate camera view matrices");
    calcView(camera);
    calcProjView(camera);

    // Render
    //GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    DEBUG_LOG("    Clear Screen");
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    //glClear(GL_COLOR_BUFFER_BIT);
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    DEBUG_LOG("    Load Renderer");
    GL_CALL(glUseProgram(render->program));


    DEBUG_LOG("    Set lighting uniforms");
    vec3 lightPositions[lightSize];
    vec3 lightColors[lightSize];

    for (int i = 0; i < lightSize; i++) {
        vec3_dup(lightPositions[i], light[i].worldPos);
        vec3_dup(lightColors[i], light[i].lightColor);
    }

    GL_CALL(int lightNumberLoc = glGetUniformLocation(render->program, "lightNumber"));
    GL_CALL(glUniform1i(lightNumberLoc, lightSize));


    GL_CALL(int lightPosLoc = glGetUniformLocation(render->program, "lightPos"));
    GL_CALL(glUniform3fv(lightPosLoc, lightSize, lightPositions[0]));

    GL_CALL(int lightColLoc = glGetUniformLocation(render->program, "lightCol"));
    GL_CALL(glUniform3fv(lightColLoc, lightSize, lightColors[0]));
    //printf("1:%f, 2:%f, 3:%f\n", lightColors[0][0], lightColors[0][1], lightColors[0][2]);

    GL_CALL(int viewPosLoc = glGetUniformLocation(render->program, "viewPos"));
    GL_CALL(glUniform3fv(viewPosLoc, 1, camera->position));


    for (int i = 0; i < modelSize; i++) {
        DEBUG_LOG("    Rendering model:%i", i);
        DEBUG_LOG("        Recalculate mvp matrix");
        mat4x4 mvp;
        mat4x4_identity(mvp);
        mat4x4_mul(mvp, camera->projViewMat, models[i].modelMat);

        DEBUG_LOG("        Set model uniforms");

        GL_CALL(int mvpLoc = glGetUniformLocation(render->program, "mvp"));
        GL_CALL(glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]));

        GL_CALL(int modelLoc = glGetUniformLocation(render->program, "model"));
        GL_CALL(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &models[i].modelMat[0][0]));

        GL_CALL(int specularStrengthLoc = glGetUniformLocation(render->program, "specularStr"));
        GL_CALL(glUniform1fv(specularStrengthLoc, 1, &models[i].specularStrength));

        DEBUG_LOG("        Render triangels");
        GL_CALL(glBindVertexArray(models[i].VAO));
        GL_CALL(glDrawElements(GL_TRIANGLES, models[i].indicesSize, GL_UNSIGNED_INT, 0));
    }
}