#include "well.h"

#define GRAVITY -9.8
#define MAX_COLLISIONS 10

void waitForFrameEnd(struct GameState* gameState) {
    GL_CALL(gameState->computationEndTime = glfwGetTime());
    double extraTime = (FRAME_RATE_LOCK - (gameState->computationEndTime - gameState->computationStartTime));
    if (extraTime < 0) {
        #if (LIGHT_DEBUG && !DEBUG)
            printf("Unable to compleat tasks in %f seconds.\n", FRAME_RATE_LOCK);
            printf("    It took %f seconds.\n", (gameState->computationEndTime - gameState->computationStartTime));
        #else
            DEBUG_LOG("Unable to compleat tasks in %f seconds.\n", FRAME_RATE_LOCK);
            DEBUG_LOG("    It took %f seconds.\n", (gameState->computationEndTime - gameState->computationStartTime));
        #endif
    } else {
        #if (LIGHT_DEBUG && !DEBUG)
            printf("Compleat tasks with %f seconds left over.\n", extraTime);
        #else
            DEBUG_LOG("Compleat tasks with %f seconds left over.\n", extraTime);
        #endif
        do {
            GL_CALL(gameState->computationEndTime = glfwGetTime());
            extraTime = (FRAME_RATE_LOCK - (gameState->computationEndTime - gameState->computationStartTime));
        } while (extraTime > 0);
    }

    gameState->computationStartTime = glfwGetTime();
}

void initPhysics(struct GameState* gameState) {
    DEBUG_LOG("Init Physics");
    dInitODE();
    gameState->world = dWorldCreate();
    gameState->space = dHashSpaceCreate(0);
    //gameState->space = dSimpleSpaceCreate(0);
    dWorldSetGravity(gameState->world, 0, 0, GRAVITY);
    dWorldSetCFM(gameState->world, 1e-5);

    gameState->contactGroup = dJointGroupCreate(0);
}

void initPhysicsObjSphere(struct Model* model, double mass, double radius, struct GameState* gameState, bool body) {
    DEBUG_LOG("Init Physics Obj Sphere");
    model->physics = true;
    
    model->geom = dCreateSphere(gameState->space, radius);
    if (body) {
        model->body = dBodyCreate(gameState->world);
        dMassSetSphere(&model->mass, mass, radius);
        dBodySetMass(model->body, &model->mass);
        dGeomSetBody(model->geom, model->body);
    }
    //dGeomSetData(model->geom, gameState);
    dGeomSetPosition(model->geom, model->worldPos[2], model->worldPos[0], model->worldPos[1]);
}

void initPhysicsObjBox(struct Model* model, double mass, double lx, double ly, double lz, struct GameState* gameState, bool body) {
    DEBUG_LOG("Init Physics Obj Box");
    model->physics = true;

    model->geom = dCreateBox(gameState->space, lx, ly, lz, 0);
    if (body) {
        model->body = dBodyCreate(gameState->world);
        dMassSetZero(&model->mass);
        model->mass.mass = mass;
        dBodySetMass(model->body, &model->mass);
        dGeomSetBody(model->geom, model->body);
    }
    //dGeomSetData(model->geom, gameState);
    dGeomSetPosition(model->geom, model->worldPos[2], model->worldPos[0], model->worldPos[1]);
}

static void nearCollisionCallback(void *data, dGeomID object1, dGeomID object2) {
    dBodyID body1 = dGeomGetBody(object1);
    dBodyID body2 = dGeomGetBody(object2);
    dContact contact;  
    contact.surface.mode = dContactBounce | dContactSoftCFM;
    // = dContactSoftERP | dContactSoftCFM | dContactApprox1 | dContactSlip1 | dContactSlip2;
    //contacts[i].surface.mu = 50.0;
    //contacts[i].surface.soft_erp = 0.96;
    //contacts[i].surface.soft_cfm = 2.00;




    // friction parameter
    contact.surface.mu = dInfinity;
    // bounce is the amount of "bouncyness".
    contact.surface.bounce = 0.9;
    // bounce_vel is the minimum incoming velocity to cause a bounce
    contact.surface.bounce_vel = 0.1;
    // constraint force mixing parameter
    contact.surface.soft_cfm = 0.001;  

    int numberOfContacts = dCollide(object1, object2, 1, &contact.geom, sizeof(dContact));

    if (numberOfContacts > 0) {
        dJointID jointContact = dJointCreateContact(((struct GameState*) data)->world, ((struct GameState*) data)->contactGroup, &contact);
        dJointAttach(jointContact, body1, body2);
    }
}

void calcPhysicsCollisions(struct GameState* gameState) {
    dSpaceCollide(gameState->space, gameState, &nearCollisionCallback);
}

void stepPhysics(struct GameState* gameState, double stepSize) {
    DEBUG_LOG("Step Physics:%f", stepSize);
    calcPhysicsCollisions(gameState);
    dWorldQuickStep(gameState->world, stepSize);
    dJointGroupEmpty(gameState->contactGroup);
}

void getPhysicsObjData(struct Model* model) {
    DEBUG_LOG("Get Physics Data");
    const double* position;
    const double* rotation;
    position = dBodyGetPosition(model->body);
    // Z is up coverted to Y is up
    defVec3(&model->worldPos, position[1], position[2], position[0]);
    //printf("x:%fy:%fz:%f\n", model->worldPos[0], model->worldPos[1], model->worldPos[2]);
    rotation = dBodyGetRotation(model->body);
    defVec3(&model->worldRot, rotation[0], rotation[1], rotation[2]);
}

void freePhysics(struct GameState* gameState) {
    DEBUG_LOG("Get Physics Data");
    dWorldDestroy(gameState->world);
    dCloseODE();
}