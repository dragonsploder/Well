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
    
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Well", 0, 0);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        exit(0);
    }

    glfwMakeContextCurrent(window);
    // Vsync
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
    initRender(&mainRender, "../../resources/shaders/main.vs", "../../resources/shaders/main.fs", "../../resources/shaders/shadow.vs", "../../resources/shaders/shadow.fs", "../../resources/shaders/shadow.gs");

    
    //vec3 pallet[4] = {50, 50, 57, 58, 99, 81, 228, 130, 87, 242, 237, 215};
    //setCurrentPallet(&mainRender, pallet);

    //struct Render outlineRender;
    //initRender(&outlineRender, "resources/shaders/main.vs", "resources/shaders/outline.fs");

    struct Camera camera;
    initCamera(&camera, 1.57f, WINDOW_WIDTH, WINDOW_HEIGHT, 0.01f, 100.0f);


    struct UI ui = {0};
    initUI(&ui, window);


    struct GameState gameState;
    gameState.currentWindowWidth = WINDOW_WIDTH;
    gameState.currentWindowHeight = WINDOW_HEIGHT;
    gameState.actualToPixelConvertion = ACTUAL_TO_PIXEL_CONVETION;
    gameState.currentPixelWidth = PIXEL_WINDOW_WIDTH;
    gameState.currentPixelHeight = PIXEL_WINDOW_HEIGHT;
    float pallet[12] = {50.0/255.0, 50.0/255.0, 57.0/255.0, 58.0/255.0, 99.0/255.0, 81.0/255.0, 228.0/255.0, 130.0/255.0, 87.0/255.0, 242.0/255.0, 237.0/255.0, 215.0/255.0};
    vec3_dup(gameState.currentPallet[0], &pallet[0]);
    vec3_dup(gameState.currentPallet[1], &pallet[3]);
    vec3_dup(gameState.currentPallet[2], &pallet[6]);
    vec3_dup(gameState.currentPallet[3], &pallet[9]);
    gameState.currentFPS = 0.0f;

    initPhysics(&gameState);

    struct Model model;
    //loadModel(&model, "resources/objects/cube.obj");
    loadModel(&model, "../../resources/objects/cube.obj");
    model.worldScale = 0.8;
    model.worldPos[0] = 0.0f;
    model.worldPos[1] = 0.0f;
    reCalcModelMat(&model);

    initPhysicsObj(&model, &gameState);


    struct Model model2;
    loadModel(&model2, "../../resources/objects/plane.obj");
    model2.worldScale = 5.0;
    model2.worldPos[1] = -2.5;
    reCalcModelMat(&model2);


    struct Light light;
    initLight(&light, (vec3) {2.0f, 5.0f, -2.0f}, (vec3) {1.0f, 1.0f, 1.0f}, mainRender);


    struct Model model3;
    loadModel(&model3, "../../resources/objects/sphere.obj");
    model3.worldScale = 0.2;
    vec3_dup(model3.worldPos, light.worldPos);
    //defVec3(&model3.worldPos, 3.0f, 3.0f, -3.0f);
    reCalcModelMat(&model3);

    struct Light light2;
    initLight(&light2, (vec3) {1.0f, 1.0f, 1.0f}, (vec3) {0.5f, 0.5f, 0.5f}, mainRender);

    struct Model model4;
    loadModel(&model4, "../../resources/objects/sphere.obj");
    model4.worldScale = 0.2;
    vec3_dup(model4.worldPos, light2.worldPos);
    //defVec3(&model3.worldPos, 3.0f, 3.0f, -3.0f);
    reCalcModelMat(&model4);



    float deltaTime = 0;
    float lastFrame = 0;
    float rx = 0;
    float ry = 0;
    float theta = 0;

    double computationStartTime = 0;
    double computationEndTime = 0;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        computationStartTime = glfwGetTime();

        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        int cur_width, cur_height;
        glfwGetFramebufferSize(window, &cur_width, &cur_height);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        gameState.currentFPS = 1.0/deltaTime;
        DEBUG_LOG("Time since init:%f", currentFrame);
        DEBUG_LOG("FPS (at current frame):%f", gameState.currentFPS);



        newUIFrame(&ui, &gameState);
        gameState.currentPixelWidth = gameState.currentWindowWidth / gameState.actualToPixelConvertion;
        gameState.currentPixelHeight = gameState.currentWindowHeight / gameState.actualToPixelConvertion;
        setCurrentPalletGameState(&mainRender, &gameState);

        //printf("FPS:%f\n", 1.0/deltaTime);

        //model.worldRot[0] = rx;
        //rx += 1.0 * deltaTime;

        //model.worldRot[1] = ry;
        //ry += 0.5 * deltaTime;
        theta += 0.5 * deltaTime;

    

        //reCalcModelMat(&model);

        //defVec3(&camera.position, 5 * cos(theta), 0.0f, 5 * sin(theta));


        stepPhysics(&gameState, FRAME_RATE_LOCK);
        getPhysicsObjData(&model);
        reCalcModelMat(&model);

        calcView(&camera);


        struct Model models[4] = {model, model2, model3, model4};

        struct Light lights[2] = {light, light2};


        renderFrame(&mainRender, &camera, &gameState, 3, models, 1, lights);
        //renderFrame(&mainRender, &camera, 1, &model2);


        renderUI(&ui);

        // Swap Buffers and Poll Events
        glfwSwapBuffers(window);
        glfwPollEvents();

        computationEndTime = glfwGetTime();
        double extraTime = (FRAME_RATE_LOCK - (computationEndTime - computationStartTime));
        if (extraTime < 0) {
            printf("Unable to compleat tasks in %d seconds.\n", FRAME_RATE_LOCK);
        } else {
            printf("Compleat tasks with %d seconds left over.\n", extraTime);
            do {
                computationEndTime = glfwGetTime();
                extraTime = (FRAME_RATE_LOCK - (computationEndTime - computationStartTime));
            } while (extraTime > 0);
        }
    }

    freePhysics(&gameState);

    freeModel(&model);
    freeRender(&mainRender);
    freeUI(&ui);

    
    terminate();
}