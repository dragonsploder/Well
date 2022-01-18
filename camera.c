#include "well.h"

void calcView(struct Camera* camera) {
    mat4x4_identity(camera->viewMat);
    mat4x4_look_at(camera->viewMat, (vec3){0.0f, 0.0f, -5.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f});
    //mat4x4_look_at(camera->viewMat, camera->position, camera->target, camera->up);
}

void calcProjection(struct Camera* camera, float FOV, int width, int height, float near, float far) {
    mat4x4_identity(camera->projMat);
    mat4x4_perspective(camera->projMat, FOV, width/height, near, far);
}

void calcProjView(struct Camera* camera) {
    //mat4x4_mul(camera->projViewMat, camera->viewMat, camera->projMat);
    mat4x4_mul(camera->projViewMat, camera->projMat, camera->viewMat);
}

void initCamera(struct Camera* camera, float FOV, int width, int height, float near, float far) {
    defVec3(&camera->position, 0.0f, 0.0f, 0.0f);
    defVec3(&camera->target, 0.0f, 0.0f, 0.0f);

    vec3 tmp;
    vec3_sub(tmp, camera->position, camera->target);
    vec3_norm(camera->direction, tmp);

    defVec3(&tmp, 0.0f, 1.0f, 0.0f);
    vec3 tmp2;
    vec3_mul_cross(tmp2, tmp, camera->direction);
    vec3_norm(camera->right, tmp2);

    defVec3(&camera->front, 0.0f, 0.0f, -1.0f);
    //up = glm::cross(direction, right);
    defVec3(&camera->up, 0.0f, 1.0f, 0.0f);


    calcView(camera);
    calcProjection(camera, FOV, width, height, near, far);
    calcProjView(camera);
}
