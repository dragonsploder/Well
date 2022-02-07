#include "well.h"

GLFWwindow* init() {
    #if DEBUG
        fclose(fopen(LOG_FILE, "w"));
    #endif



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
    glEnable(GL_CULL_FACE);

    return window;
}

void terminate() {
    glfwTerminate();
}

int main() {
    
    GLFWwindow* window = init();

    struct Render mainRender;
    initRender(&mainRender, "resources/shaders/main.vs", "resources/shaders/main.fs", "resources/shaders/shadow.vs", "resources/shaders/shadow.fs", "resources/shaders/shadow.gs");

    //struct Render outlineRender;
    //initRender(&outlineRender, "resources/shaders/main.vs", "resources/shaders/outline.fs");

    struct Camera camera;
    initCamera(&camera, 1.4f, WINDOW_WIDTH, WINDOW_HEIGHT, 0.01f, 100.0f);

    struct Model model;
    loadModel(&model, "resources/objects/cube.obj");
    //loadModel(&model, "resources/objects/arch.obj");
    model.worldScale = 0.4;
    model.worldPos[1] = -1.0f;
    reCalcModelMat(&model);


    struct Model model2;
    loadModel(&model2, "resources/objects/plane.obj");
    model2.worldScale = 5.0;
    model2.worldPos[1] = -2.5;
    reCalcModelMat(&model2);


    struct Light light;
    initLight(&light, (vec3) {2.0f, 5.0f, -2.0f}, (vec3) {1.0f, 1.0f, 1.0f}, mainRender);


    struct Model model3;
    loadModel(&model3, "resources/objects/sphere.obj");
    model3.worldScale = 0.2;
    vec3_dup(model3.worldPos, light.worldPos);
    //defVec3(&model3.worldPos, 3.0f, 3.0f, -3.0f);
    reCalcModelMat(&model3);

    struct Light light2;
    initLight(&light2, (vec3) {1.0f, 1.0f, 1.0f}, (vec3) {1.0f, 1.0f, 1.0f}, mainRender);

    struct Model model4;
    loadModel(&model4, "resources/objects/sphere.obj");
    model4.worldScale = 0.2;
    vec3_dup(model4.worldPos, light2.worldPos);
    //defVec3(&model3.worldPos, 3.0f, 3.0f, -3.0f);
    reCalcModelMat(&model4);



    float deltaTime = 0;
    float lastFrame = 0;
    float rx = 0;
    float ry = 0;
    float theta = 0;


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
        DEBUG_LOG("Time since init:%f", currentFrame);
        DEBUG_LOG("FPS (at current frame):%f", 1.0/deltaTime);
        //printf("FPS:%f\n", 1.0/deltaTime);

        //model.worldRot[0] = rx;
        //rx += 1.0 * deltaTime;

        //model.worldRot[1] = ry;
        //ry += 0.5 * deltaTime;
        theta += 0.5 * deltaTime;

        //reCalcModelMat(&model);

        defVec3(&camera.position, 5 * cos(theta), 0.0f, 5 * sin(theta));

        calcView(&camera);


        struct Model models[4] = {model, model2, model3, model4};

        struct Light lights[2] = {light, light2};


        renderFrame(&mainRender, &camera, 4, models, 2, lights);
        //renderFrame(&mainRender, &camera, 1, &model2);



        // Swap Buffers and Poll Events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    freeModel(&model);
    freeRender(&mainRender);


    terminate();
}