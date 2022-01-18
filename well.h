#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "linmath.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>



#define DEBUG true

#ifdef DEBUG
    #define GL_CALL(x) glClearErrors(); x; glCheckError(__FILE__, __LINE__, #x);
    extern void glClearErrors();
    extern void glCheckError(const char* file, int line, const char* function);
#else
    #define GL_CALL(x) x
#endif


struct Camera {
    mat4x4 viewMat;
    mat4x4 projMat;
    mat4x4 projViewMat;

    vec3 position;
    vec3 target;
    vec3 direction;
    vec3 right;
    vec3 front;
    vec3 up;


    // Properties (x,y,z)
    vec3 worldPos;
    vec3 worldRot;
};


struct Render {
    unsigned int program;
};


struct Model {
    // Mesh data
    int verticesSize;
    float* vertices;
    int indicesSize;
    unsigned int* indices;

    // OpenGl mesh data
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    // Properties (x,y,z)
    vec3 worldPos;
    vec3 worldRot;
    float worldScale;

    mat4x4 modelMat;
};



// camera.c
extern void calcView(struct Camera* camera);
extern void calcProjection(struct Camera* camera, float FOV, int width, int height, float near, float far);
extern void calcProjView(struct Camera* Camera);
extern void initCamera(struct Camera* camera, float FOV, int width, int height, float near, float far);

// render.c
extern void initRender(struct Render* render, char vertexPath[500], char fragmentPath[500]);
extern void freeRender(struct Render* render);
extern void renderFrame(struct Render* render, struct Camera* camera, int modelSize, struct Model model[]);

// model.c 
extern void reCalcModelMat(struct Model* model);
extern void loadModel(struct Model* model, char path[500]);
extern void freeModel(struct Model* model);

// misc.c
extern void defVec3(vec3* vec, float x, float y, float z);
extern bool readFile(char path[200], char** fileData, size_t* size);
