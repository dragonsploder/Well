#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "linmath.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1200


#define DEBUG true
#define LOG_FILE "log.txt"

#ifdef DEBUG
    #define GL_CALL(x) glClearErrors(); x; glCheckError(__FILE__, __LINE__, #x);
    extern void glClearErrors();
    extern void glCheckError(const char* file, int line, const char* function);

    //#define DEBUG_LOG(...) printf(__VA_ARGS__);
    #define DEBUG_LOG(...) { FILE* logFile = fopen(LOG_FILE, "a"); fprintf(logFile, __VA_ARGS__); fprintf(logFile, "  -- File:%s, Line:%i\n", __FILE__, __LINE__); fclose(logFile); }
#else
    #define GL_CALL(x) x

    #define DEBUG_LOG(...)
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
    unsigned int mainProgram;
    unsigned int shadowProgram;

    unsigned int shadowWidth;
    unsigned int shadowHeight;

    unsigned int depthMapFBO[10];
    unsigned int depthCubemap[10];
    mat4x4 shadowProj;
    float shadowAspect;
    float shadowNear;
    float shadowFar;
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
    float specularStrength;
};

struct Light {
    vec3 worldPos;

    vec3 lightColor;

    mat4x4 shadowTransforms[6];
};



// camera.c
extern void calcView(struct Camera* camera);
extern void calcProjection(struct Camera* camera, float FOV, int width, int height, float near, float far);
extern void calcProjView(struct Camera* Camera);
extern void initCamera(struct Camera* camera, float FOV, int width, int height, float near, float far);

// render.c
extern void initShader(unsigned int* shader, unsigned int shaderType, char path[500]);
extern void initRender(struct Render* render, char mainVertexPath[500], char mainFragmentPath[500], char shadowVertexPath[500], char shadowFragmentPath[500], char shadowGeometryPath[500]);
extern void freeRender(struct Render* render);
extern void renderActual(struct Render* render, struct Camera* camera, int modelSize, struct Model models[]);
extern void renderFrame(struct Render* render, struct Camera* camera, int modelSize, struct Model models[], int lightSize, struct Light light[]);

// model.c 
extern void reCalcModelMat(struct Model* model);
extern void loadModel(struct Model* model, char path[500]);
extern void freeModel(struct Model* model);

// light.c
extern void reCalcLight(struct Light* light, struct Render render);
extern void initLight(struct Light* light, vec3 pos, vec3 color, struct Render render);

// misc.c
extern void defVec3(vec3* vec, float x, float y, float z);
extern bool readFile(char path[200], char** fileData, size_t* size);
