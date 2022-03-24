#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "linmath.h"


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"


#define dDOUBLE
#include "ode/ode.h"

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1200


#define ACTUAL_TO_PIXEL_CONVETION 5
#define PIXEL_WINDOW_WIDTH (WINDOW_WIDTH / ACTUAL_TO_PIXEL_CONVETION)
#define PIXEL_WINDOW_HEIGHT (WINDOW_HEIGHT / ACTUAL_TO_PIXEL_CONVETION)

#define FPS_GOAL 30.0f
#define FRAME_RATE_LOCK (1.0f / FPS_GOAL)

#define MAX_LIGHTS 10

#define VSYNC 0


#define DEBUG true
#define LIGHT_DEBUG true /* Few critical printfs (only relevent if DEBUG is false) */
#define DEBUG_TO_LOG_FILE true /* Print to file instead of console (very slow) */
#define LOG_FILE "log.txt"

#if DEBUG
    #undef LIGHT_DEBUG
    #define LIGHT_DEBUG true

    #define GL_CALL(x) glClearErrors(); x; glCheckError(__FILE__, __LINE__, #x);
    extern void glClearErrors();
    extern void glCheckError(const char* file, int line, const char* function);

    #if DEBUG_TO_LOG_FILE
        #define DEBUG_LOG(...) { FILE* logFile = fopen(LOG_FILE, "a"); fprintf(logFile, __VA_ARGS__); fprintf(logFile, "  -- File:%s, Line:%i\n", __FILE__, __LINE__); fclose(logFile); }
    #else 
        #define DEBUG_LOG(...) { printf(__VA_ARGS__); printf("  -- File:%s, Line:%i\n", __FILE__, __LINE__); }
    #endif
#else
    #define GL_CALL(x) x

    #define DEBUG_LOG(...)
#endif

struct GameState {
    int currentWindowWidth;
    int currentWindowHeight;

    int actualToPixelConvertion;
    int currentPixelWidth;
    int currentPixelHeight;

    vec3 currentPallet[4];

    float currentFPS;

    double computationStartTime;
    double computationEndTime;



    dWorldID world;
    dSpaceID space;


    dJointGroupID contactGroup;
};


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
    // vec3 worldPos;
    // vec3 worldRot;
};


struct Render {
    unsigned int currentWidth;
    unsigned int currentHeight;
    unsigned int currentPixelWidth;
    unsigned int currentPixelHeight;


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

    unsigned int scaleFramebuffer;
    vec3 pallet[4];
};

struct UI {
    struct nk_glfw glfw;
//    struct nk_colorf bg;
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

    // Physics
    bool physics;
    dBodyID body;
    dGeomID geom;	
    dMass mass;
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

extern void setCurrentPalletDirect(struct Render* render, vec3 colors[4]);
extern void setCurrentPalletGameState(struct Render* render, struct GameState* gameState);
extern void initShader(unsigned int* shader, unsigned int shaderType, char path[500]);
extern void initRender(struct Render* render, char mainVertexPath[500], char mainFragmentPath[500], char shadowVertexPath[500], char shadowFragmentPath[500], char shadowGeometryPath[500]);
extern void freeRender(struct Render* render);
extern void renderActual(struct Render* render, struct Camera* camera, int modelSize, struct Model models[]);
extern void renderFrame(struct Render* render, struct Camera* camera, struct GameState* gameState, int modelSize, struct Model models[], int lightSize, struct Light light[]);

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


// ui.c
extern void initUI(struct UI* ui, GLFWwindow* window);
extern void newUIFrame(struct UI* ui, struct GameState* gameState);
extern void renderUI(struct UI* ui);
extern void freeUI(struct UI* ui);

// physics.c
extern void waitForFrameEnd(struct GameState* gameState);
extern void initPhysics(struct GameState* gameState);
extern void initPhysicsObjSphere(struct Model* model, double mass, double radius, struct GameState* gameState, bool body);
extern void initPhysicsObjBox(struct Model* model, double mass, double lx, double ly, double lz, struct GameState* gameState, bool body);
extern void stepPhysics(struct GameState* gameState, double stepSize);
extern void getPhysicsObjData(struct Model* model);
extern void freePhysics(struct GameState* gameState);