#include "well.h"

void setCurrentPalletDirect(struct Render* render, vec3 colors[4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            colors[i][j] /= 255.0;
        }
    }

    vec3_dup(render->pallet[0], colors[0]);
    vec3_dup(render->pallet[1], colors[1]);
    vec3_dup(render->pallet[2], colors[2]);
    vec3_dup(render->pallet[3], colors[3]);
}

void setCurrentPalletGameState(struct Render* render, struct GameState* gameState) {
    vec3_dup(render->pallet[0], gameState->currentPallet[0]);
    vec3_dup(render->pallet[1], gameState->currentPallet[1]);
    vec3_dup(render->pallet[2], gameState->currentPallet[2]);
    vec3_dup(render->pallet[3], gameState->currentPallet[3]);
}

void initShader(unsigned int* shader, unsigned int shaderType, char path[500]) {
    char *shaderSource;
    size_t shaderSourceSize;
    DEBUG_LOG("Reading shader");
    readFile(path, &shaderSource, &shaderSourceSize);
    const char* shaderSourceConst = shaderSource;

    DEBUG_LOG("Compiling shader");
    GL_CALL(*shader = glCreateShader(shaderType));
    GL_CALL(glShaderSource(*shader, 1, &shaderSourceConst, NULL));
    GL_CALL(glCompileShader(*shader));
    // check for shader compile errors
    int success;
    char infoLog[10000];
    GL_CALL(glGetShaderiv(*shader, GL_COMPILE_STATUS, &success));
    if (!success) {
        glGetShaderInfoLog(*shader, 10000, NULL, infoLog);
        printf("Shader Compilation Failed\n");
        printf("%s\n", infoLog);
        exit(0);
    }
}

void initRender(struct Render* render, char mainVertexPath[500], char mainFragmentPath[500], char shadowVertexPath[500], char shadowFragmentPath[500], char shadowGeometryPath[500]) {
    DEBUG_LOG("Init render");


    render->currentWidth = WINDOW_WIDTH;
    render->currentHeight = WINDOW_HEIGHT;

    render->currentPixelWidth = PIXEL_WINDOW_WIDTH;
    render->currentPixelHeight = PIXEL_WINDOW_HEIGHT;

    unsigned int vertexShader;
    DEBUG_LOG("Init vertex shader");
    initShader(&vertexShader, GL_VERTEX_SHADER, mainVertexPath);

    unsigned int fragmentShader;
    DEBUG_LOG("Init fragment shader");
    initShader(&fragmentShader, GL_FRAGMENT_SHADER, mainFragmentPath);


    // link main shaders
    DEBUG_LOG("Linking shaders to program");
    GL_CALL(render->mainProgram = glCreateProgram());
    GL_CALL(glAttachShader(render->mainProgram, vertexShader));
    GL_CALL(glAttachShader(render->mainProgram, fragmentShader));
    GL_CALL(glLinkProgram(render->mainProgram));
    // check for linking errors
    int success;
    GL_CALL(glGetProgramiv(render->mainProgram, GL_LINK_STATUS, &success));
    if (!success) {
        char infoLog[10000];
        glGetShaderInfoLog(render->mainProgram, 10000, NULL, infoLog);
        printf("Shader Program Compilaton Error\n");
        printf("%s\n", infoLog);
        exit(0);
    }
    DEBUG_LOG("Deleting shaders");
    GL_CALL(glDeleteShader(vertexShader));
    GL_CALL(glDeleteShader(fragmentShader));




    DEBUG_LOG("Init vertex shader");
    initShader(&vertexShader, GL_VERTEX_SHADER, shadowVertexPath);

    DEBUG_LOG("Init fragment shader");
    initShader(&fragmentShader, GL_FRAGMENT_SHADER, shadowFragmentPath);

    unsigned int geometryShader;
    DEBUG_LOG("Init fragment shader");
    initShader(&geometryShader, GL_GEOMETRY_SHADER, shadowGeometryPath);


    // link main shaders
    DEBUG_LOG("Linking shaders to program");
    GL_CALL(render->shadowProgram = glCreateProgram());
    GL_CALL(glAttachShader(render->shadowProgram, vertexShader));
    GL_CALL(glAttachShader(render->shadowProgram, fragmentShader));
    GL_CALL(glAttachShader(render->shadowProgram, geometryShader));
    GL_CALL(glLinkProgram(render->shadowProgram));
    // check for linking errors
    GL_CALL(glGetProgramiv(render->shadowProgram, GL_LINK_STATUS, &success));
    if (!success) {
        char infoLog[10000];
        glGetShaderInfoLog(render->shadowProgram, 10000, NULL, infoLog);
        printf("Shader Program Compilaton Error\n");
        printf("%s\n", infoLog);
        exit(0);
    }
    DEBUG_LOG("Deleting shaders");
    GL_CALL(glDeleteShader(vertexShader));
    GL_CALL(glDeleteShader(fragmentShader));
    GL_CALL(glDeleteShader(geometryShader));





    /* Lighting Init */
    render->shadowWidth = 1024;
    render->shadowHeight = 1024;

    for (int i = 0; i < 10; i++) {
        glGenFramebuffers(1, &render->depthMapFBO[i]);


        glGenTextures(1, &render->depthCubemap[i]);

        glBindTexture(GL_TEXTURE_CUBE_MAP, render->depthCubemap[i]);
        for (unsigned int j = 0; j < 6; j++) {
            GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT, render->shadowWidth, render->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));  
        }


        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

        glBindFramebuffer(GL_FRAMEBUFFER, render->depthMapFBO[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, render->depthCubemap[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    }


    render->shadowAspect = (float)render->shadowWidth/(float)render->shadowHeight;
    render->shadowNear = 1.0f;
    render->shadowFar = 25.0f;
    //glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far); 
    mat4x4_perspective(render->shadowProj, 1.5708f, render->shadowAspect, render->shadowNear, render->shadowFar);


    glGenFramebuffers(1, &render->scaleFramebuffer);

    unsigned int color;
    unsigned int depth;
    glGenTextures(1, &color);
    glGenRenderbuffers(1, &depth);


    glBindFramebuffer(GL_FRAMEBUFFER, render->scaleFramebuffer);

    glBindTexture(GL_TEXTURE_2D, color);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, render->currentWidth, render->currentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, render->currentWidth, render->currentHeight);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);


    vec3 pallet[4] = {20, 20, 20, 100, 100, 100, 150, 150, 150, 220, 220, 220};

    setCurrentPalletDirect(render, pallet);
}


void freeRender(struct Render* render) {
    DEBUG_LOG("Free renderer");
    GL_CALL(glDeleteProgram(render->mainProgram));
    GL_CALL(glDeleteProgram(render->shadowProgram));
    GL_CALL(glDeleteTextures(10, &render->depthCubemap[0]));
}

void renderFrame(struct Render* render, struct Camera* camera, struct GameState* gameState, int modelSize, struct Model models[], int lightSize, struct Light light[]) {
    render->currentWidth = gameState->currentWindowWidth;
    render->currentHeight = gameState->currentWindowHeight;

    render->currentPixelWidth = gameState->currentPixelWidth;
    render->currentPixelHeight = gameState->currentPixelHeight;
    
    
    
    DEBUG_LOG("Render Frame\n");
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Render
    //GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    DEBUG_LOG("    Clear Screen");
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    //glClear(GL_COLOR_BUFFER_BIT);
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    DEBUG_LOG("    Recalculate camera view matrices");
    calcView(camera);
    calcProjView(camera);

    glViewport(0, 0, render->shadowWidth, render->shadowHeight);



    vec3 lightPositions[MAX_LIGHTS];
    vec3 lightColors[MAX_LIGHTS];


    for (int i = 0; i < lightSize; i++) {
        vec3_dup(lightPositions[i], light[i].worldPos);
        vec3_dup(lightColors[i], light[i].lightColor);

        glBindFramebuffer(GL_FRAMEBUFFER, render->depthMapFBO[i]);
        glClear(GL_DEPTH_BUFFER_BIT);

        DEBUG_LOG("    Load Shadow Renderer");
        GL_CALL(glUseProgram(render->shadowProgram));
        GL_CALL(int shadowMatricesLocShadow = glGetUniformLocation(render->shadowProgram, "shadowMatrices"));
        GL_CALL(glUniformMatrix4fv(shadowMatricesLocShadow, 6, false, &light[i].shadowTransforms[0][0][0]));


        GL_CALL(int farPlaneLocShadow = glGetUniformLocation(render->shadowProgram, "farPlane"));
        GL_CALL(glUniform1f(farPlaneLocShadow, render->shadowFar));


        GL_CALL(int lightPosLocShadow = glGetUniformLocation(render->shadowProgram, "lightPos"));
        GL_CALL(glUniform3fv(lightPosLocShadow, 1, lightPositions[i]));

        for (int i = 0; i < modelSize; i++) {
            /*DEBUG_LOG("    Rendering model:%i", i);
            DEBUG_LOG("        Recalculate mvp matrix");
            mat4x4 mvp;
            mat4x4_identity(mvp);
            mat4x4_mul(mvp, camera->projViewMat, models[i].modelMat);

            DEBUG_LOG("        Set model uniforms");

            GL_CALL(int mvpLoc = glGetUniformLocation(render->mainProgram, "mvp"));
            GL_CALL(glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]));*/

            GL_CALL(int modelLoc = glGetUniformLocation(render->shadowProgram, "model"));
            GL_CALL(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &models[i].modelMat[0][0]));

            DEBUG_LOG("        Render triangels");
            GL_CALL(glBindVertexArray(models[i].VAO));
            GL_CALL(glDrawElements(GL_TRIANGLES, models[i].indicesSize, GL_UNSIGNED_INT, 0));
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }



    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_TEXTURE_2D);
    glBindFramebuffer(GL_FRAMEBUFFER, render->scaleFramebuffer);



    glViewport(0, 0, render->currentPixelWidth, render->currentPixelHeight);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DEBUG_LOG("    Load Main Renderer");
    GL_CALL(glUseProgram(render->mainProgram));

    int depthMapIndexes[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    for (int i = 0; i < lightSize; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, render->depthCubemap[i]);
    }


    DEBUG_LOG("    Set lighting uniforms");

    GL_CALL(int lightNumberLoc = glGetUniformLocation(render->mainProgram, "lightNumber"));
    GL_CALL(glUniform1i(lightNumberLoc, lightSize));

    GL_CALL(int depthMapLoc = glGetUniformLocation(render->mainProgram, "depthMap"));
    glUniform1iv(depthMapLoc, lightSize, &depthMapIndexes[0]);

    GL_CALL(int farPlaneLoc = glGetUniformLocation(render->mainProgram, "farPlane"));
    GL_CALL(glUniform1f(farPlaneLoc, render->shadowFar));


    GL_CALL(int lightPosLoc = glGetUniformLocation(render->mainProgram, "lightPos"));
    GL_CALL(glUniform3fv(lightPosLoc, lightSize, lightPositions[0]));

    GL_CALL(int lightColLoc = glGetUniformLocation(render->mainProgram, "lightCol"));
    GL_CALL(glUniform3fv(lightColLoc, lightSize, lightColors[0]));
    //printf("1:%f, 2:%f, 3:%f\n", lightColors[0][0], lightColors[0][1], lightColors[0][2]);

    GL_CALL(int viewPosLoc = glGetUniformLocation(render->mainProgram, "viewPos"));
    GL_CALL(glUniform3fv(viewPosLoc, 1, camera->position));


    GL_CALL(int palletLoc = glGetUniformLocation(render->mainProgram, "pallet"));
    GL_CALL(glUniform3fv(palletLoc, 4, render->pallet[0]));


    for (int i = 0; i < modelSize; i++) {
        DEBUG_LOG("    Rendering model:%i", i);
        DEBUG_LOG("        Recalculate mvp matrix");
        mat4x4 mvp;
        mat4x4_identity(mvp);
        mat4x4_mul(mvp, camera->projViewMat, models[i].modelMat);

        DEBUG_LOG("        Set model uniforms");

        GL_CALL(int mvpLoc = glGetUniformLocation(render->mainProgram, "mvp"));
        GL_CALL(glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]));

        GL_CALL(int modelLoc = glGetUniformLocation(render->mainProgram, "model"));
        GL_CALL(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &models[i].modelMat[0][0]));

        GL_CALL(int specularStrengthLoc = glGetUniformLocation(render->mainProgram, "specularStr"));
        GL_CALL(glUniform1f(specularStrengthLoc, models[i].specularStrength));

        DEBUG_LOG("        Render triangels");
        GL_CALL(glBindVertexArray(models[i].VAO));
        GL_CALL(glDrawElements(GL_TRIANGLES, models[i].indicesSize, GL_UNSIGNED_INT, 0));
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, render->scaleFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(0, 0, render->currentPixelWidth, render->currentPixelHeight, 0, 0, render->currentWidth, render->currentHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}