#include "well.h"

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1200

GLFWwindow* init() {
    /* Create window and GL context via GLFW */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Well", 0, 0);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        exit(0);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Load Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("Failed to initialize GLAD");
        exit(0);
    }    

    glEnable(GL_DEPTH_TEST);

    return window;
}

void terminate() {
    glfwTerminate();
}

int main() {
    
    GLFWwindow* window = init();

    struct Render render;
    initRender(&render, "resources/shaders/main.vs", "resources/shaders/main.fs");

    struct Camera camera;
    initCamera(&camera, 1.4f, WINDOW_WIDTH, WINDOW_HEIGHT, 0.01f, 10.0f );

    struct Model model;
    loadModel(&model, "resources/objects/cube.obj");


    float deltaTime = 0;
    float lastFrame = 0;
    float rx = 0;
    float ry = 0;


    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        int cur_width, cur_height;
        glfwGetFramebufferSize(window, &cur_width, &cur_height);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //printf("deltaTime:%f\n", deltaTime);

        model.worldRot[0] = rx;
        rx += 1.0 * deltaTime;

        model.worldRot[1] = ry;
        ry += 0.5 * deltaTime;

        reCalcModelMat(&model);


        renderFrame(&render, &camera, 1, &model);



        // Swap Buffers and Poll Events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    freeModel(&model);
    freeRender(&render);


    terminate();
}