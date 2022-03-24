#include "well.h"

GLFWwindow* init() {
    /* Create debug log file */
    #if DEBUG && DEBUG_TO_LOG_FILE
        {
            FILE* debugFile = fopen(LOG_FILE, "w");
            fprintf(debugFile, "Well Debug Log File\n\n");
            fclose(debugFile);
        }
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
        DEBUG_LOG("Failed to create GLFW window\n");
        glfwTerminate();
        exit(0);
    }

    glfwMakeContextCurrent(window);

    /* Vsync */
    glfwSwapInterval(VSYNC);

    /* Load Glad */
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
        DEBUG_LOG("Failed to initialize GLAD");
        exit(0);
    }    
    
    /* Enable addition settings */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    return window;
}

void terminate() {
    glfwTerminate();
}

int main() {
    srand(time(NULL));
    GLFWwindow* window = init();

    struct Render mainRender;
    initRender(&mainRender, "../../resources/shaders/main.vs", "../../resources/shaders/main.fs", "../../resources/shaders/shadow.vs", "../../resources/shaders/shadow.fs", "../../resources/shaders/shadow.gs");

    
    //vec3 pallet[4] = {50, 50, 57, 58, 99, 81, 228, 130, 87, 242, 237, 215};
    //setCurrentPallet(&mainRender, pallet);

    struct Camera camera;
    initCamera(&camera, 1.57f, WINDOW_WIDTH, WINDOW_HEIGHT, 0.01f, 100.0f);
    defVec3(&camera.position, 5.0f, 3.0f, 3.0f);
    calcView(&camera);
    calcProjView(&camera);


    struct UI ui = {0};
    initUI(&ui, window);


    struct GameState gameState;
    gameState.currentWindowWidth = WINDOW_WIDTH;
    gameState.currentWindowHeight = WINDOW_HEIGHT;
    gameState.actualToPixelConvertion = ACTUAL_TO_PIXEL_CONVETION;
    gameState.currentPixelWidth = PIXEL_WINDOW_WIDTH;
    gameState.currentPixelHeight = PIXEL_WINDOW_HEIGHT;
    gameState.computationStartTime = 0;
    gameState.computationEndTime = 0;
    float pallet[12] = {50.0/255.0, 50.0/255.0, 57.0/255.0, 58.0/255.0, 99.0/255.0, 81.0/255.0, 228.0/255.0, 130.0/255.0, 87.0/255.0, 242.0/255.0, 237.0/255.0, 215.0/255.0};
    vec3_dup(gameState.currentPallet[0], &pallet[0]);
    vec3_dup(gameState.currentPallet[1], &pallet[3]);
    vec3_dup(gameState.currentPallet[2], &pallet[6]);
    vec3_dup(gameState.currentPallet[3], &pallet[9]);
    gameState.currentFPS = 0.0f;

    initPhysics(&gameState);

    struct Model model;
    //loadModel(&model, "resources/objects/cube.obj");
    loadModel(&model, "../../resources/objects/sphere.obj");
    model.worldScale = 0.8;
    model.worldPos[1] = 2.0f;
    reCalcModelMat(&model);

    initPhysicsObjSphere(&model, 1.0, 0.5, &gameState, true);
    //dBodySetLinearVel(model.body, ((float) (rand() % 100)) / 100.0f, 0.0, 0.0);


    struct Model model2;
    loadModel(&model2, "../../resources/objects/plane.obj");
    model2.worldScale = 5.0;
    model2.worldPos[1] = -2.5;
    reCalcModelMat(&model2);

    initPhysicsObjBox(&model2, 1.0, 10.0, 10.0, 2.0, &gameState, false);


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
    float theta = 0;

    double computationStartTime = 0;
    double computationEndTime = 0;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        int cur_width, cur_height;
        GL_CALL(glfwGetFramebufferSize(window, &cur_width, &cur_height));
        GL_CALL(float currentFrame = glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        gameState.currentFPS = 1.0/deltaTime;
        DEBUG_LOG("Time since init:%f", currentFrame);
        DEBUG_LOG("FPS (at current frame):%f", gameState.currentFPS);



        newUIFrame(&ui, &gameState);
        gameState.currentPixelWidth = gameState.currentWindowWidth / gameState.actualToPixelConvertion;
        gameState.currentPixelHeight = gameState.currentWindowHeight / gameState.actualToPixelConvertion;
        setCurrentPalletGameState(&mainRender, &gameState);

        theta += 0.5 * deltaTime;

        if (rand() % 500 == 0) {
            dBodySetLinearVel(model.body, 0.0, 0.0, ((float) (rand() % 100)) / 20.0f + 5.0f);
        }

        stepPhysics(&gameState, FRAME_RATE_LOCK);
        getPhysicsObjData(&model);
        reCalcModelMat(&model);

        //calcView(&camera);


        struct Model models[4] = {model, model2, model3, model4};

        struct Light lights[2] = {light, light2};


        renderFrame(&mainRender, &camera, &gameState, 3, models, 1, lights); /* Multiple lights broken -- need to fix */


        renderUI(&ui);

        /* Swap Buffers and Poll Events */
        GL_CALL(glfwSwapBuffers(window));
        GL_CALL(glfwPollEvents());

        waitForFrameEnd(&gameState);
    }

    freePhysics(&gameState);

    freeModel(&model);
    freeRender(&mainRender);
    freeUI(&ui);

    
    terminate();
}