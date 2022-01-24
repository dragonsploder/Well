#include "well.h"

void initLight(struct Light* light, vec3 pos, vec3 color) {
    DEBUG_LOG("Init light with pos[0]:%f, pos[1]:%f, pos[2]:%f, color[0]:%f, color[1]:%f, color[2]:%f", pos[0], pos[1], pos[2], color[0], color[1], color[2]);
    vec3_dup(light->worldPos, pos);
    vec3_dup(light->lightColor, color);
}