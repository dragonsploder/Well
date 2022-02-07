#include "well.h"

void reCalcLight(struct Light* light, struct Render render) {
    vec3 tmp;

    vec3_add(tmp, light->worldPos, (vec3) {1.0, 0.0, 0.0});
    mat4x4_look_at(light->shadowTransforms[0], light->worldPos, tmp, (vec3) {0.0, -1.0, 0.0});
    mat4x4_mul(light->shadowTransforms[0],  render.shadowProj, light->shadowTransforms[0]);    

    vec3_add(tmp, light->worldPos, (vec3) {-1.0, 0.0, 0.0});
    mat4x4_look_at(light->shadowTransforms[1], light->worldPos, tmp, (vec3) {0.0, -1.0, 0.0});
    mat4x4_mul(light->shadowTransforms[1], render.shadowProj, light->shadowTransforms[1]);

    vec3_add(tmp, light->worldPos, (vec3) {0.0, 1.0, 0.0});
    mat4x4_look_at(light->shadowTransforms[2], light->worldPos, tmp, (vec3) {0.0, 0.0, 1.0});
    mat4x4_mul(light->shadowTransforms[2], render.shadowProj, light->shadowTransforms[2]);

    vec3_add(tmp, light->worldPos, (vec3) {0.0, -1.0, 0.0});
    mat4x4_look_at(light->shadowTransforms[3], light->worldPos, tmp, (vec3) {0.0, 0.0, -1.0});
    mat4x4_mul(light->shadowTransforms[3], render.shadowProj, light->shadowTransforms[3]);

    vec3_add(tmp, light->worldPos, (vec3) {0.0, 0.0, 1.0});
    mat4x4_look_at(light->shadowTransforms[4], light->worldPos, tmp, (vec3) {0.0, -1.0, 0.0});
    mat4x4_mul(light->shadowTransforms[4], render.shadowProj, light->shadowTransforms[4]);

    vec3_add(tmp, light->worldPos, (vec3) {0.0, 0.0, -1.0});
    mat4x4_look_at(light->shadowTransforms[5], light->worldPos, tmp, (vec3) {0.0, -1.0, 0.0});
    mat4x4_mul(light->shadowTransforms[5], render.shadowProj, light->shadowTransforms[5]);
}

void initLight(struct Light* light, vec3 pos, vec3 color, struct Render render) {
    DEBUG_LOG("Init light with pos[0]:%f, pos[1]:%f, pos[2]:%f, color[0]:%f, color[1]:%f, color[2]:%f", pos[0], pos[1], pos[2], color[0], color[1], color[2]);
    vec3_dup(light->worldPos, pos);
    vec3_dup(light->lightColor, color);


    reCalcLight(light, render);
}