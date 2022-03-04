#include "well.h"

#define GRAVITY 9.8

void initPhysics(struct GameState* gameState) {
    dInitODE();
   gameState->world = dWorldCreate();
   gameState->space = dHashSpaceCreate(0);
   dWorldSetGravity(gameState->world, 0, 0, GRAVITY);
   dWorldSetCFM(gameState->world, 1e-5);
}

void initPhysicsObj(struct Model* model, struct GameState* gameState) {
    float radius = 1.0f;
    float mass = 1.0f;
    model->physics = true;
    model->body = dBodyCreate(gameState->world);
    dMassSetZero(&model->mass);
    dMassSetSphereTotal(&model->mass, mass, radius);
    dBodySetMass(model->body, &model->mass);
    dBodySetPosition(model->body, model->worldPos[0], model->worldPos[1], model->worldPos[2]);
}

void stepPhysics(struct GameState* gameState, double stepSize) {
    dWorldQuickStep(gameState->world, stepSize);
}

void getPhysicsObjData(struct Model* model) {
    const double* position;
    const double* rotation;
    position = dBodyGetPosition(model->body);
    defVec3(&model->worldPos, position[0], position[1], position[2]);
    rotation = dBodyGetRotation(model->body);
    defVec3(&model->worldRot, rotation[0], rotation[1], rotation[2]);
}

void freePhysics(struct GameState* gameState) {
    dWorldDestroy(gameState->world);
    dCloseODE();
}