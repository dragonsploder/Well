#include "well.h"

void initRender(struct Render* render, char vertexPath[500], char fragmentPath[500]) {
    char *vertexShaderSource;
    size_t vertexShaderSourceSize;
    readFile(vertexPath, &vertexShaderSource, &vertexShaderSourceSize);
    const char* vertexShaderSourceConst = vertexShaderSource;

    char *fragmentShaderSource;
    size_t fragmentShaderSourceSize;
    readFile(fragmentPath, &fragmentShaderSource, &fragmentShaderSourceSize);
    const char* fragmentShaderSourceConst = fragmentShaderSource;

    // build and compile our shader program
    // vertex shader
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
    GL_CALL(glDeleteShader(vertexShader));
    GL_CALL(glDeleteShader(fragmentShader));
}


void freeRender(struct Render* render) {
    GL_CALL(glDeleteProgram(render->program));
}


void renderFrame(struct Render* render, struct Camera* camera, int modelSize, struct Model model[]) {
    calcView(camera);
    calcProjView(camera);

    // Render
    GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    //glClear(GL_COLOR_BUFFER_BIT);
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GL_CALL(glUseProgram(render->program));

    for (int i = 0; i < modelSize; i++) {

        mat4x4 mvp;
        mat4x4_identity(mvp);
        mat4x4_mul(mvp, camera->projViewMat, model[i].modelMat);

        GL_CALL(int mat4x4Loc = glGetUniformLocation(render->program, "mvp"));
        GL_CALL(glUniformMatrix4fv(mat4x4Loc, 1, GL_FALSE, &mvp[0][0]));


        GL_CALL(glBindVertexArray(model[i].VAO));
        GL_CALL(glDrawElements(GL_TRIANGLES, model[i].indicesSize, GL_UNSIGNED_INT, 0));
    }
}