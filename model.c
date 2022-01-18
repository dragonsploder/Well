#include "well.h"

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

void reCalcModelMat(struct Model* model) {
    mat4x4 ident;
    mat4x4_identity(ident);

    mat4x4 tmp;
    mat4x4_identity(tmp);

    mat4x4_identity(model->modelMat);

    mat4x4_rotate_X(tmp, ident, model->worldRot[0]);
    mat4x4_mul(model->modelMat, model->modelMat, tmp);

    mat4x4_rotate_Y(tmp, ident, model->worldRot[1]);
    mat4x4_mul(model->modelMat, model->modelMat, tmp);
    mat4x4_rotate_Z(tmp, ident, model->worldRot[2]);
    mat4x4_mul(model->modelMat, model->modelMat, tmp);

    mat4x4_scale(tmp, ident, model->worldScale);
    mat4x4_mul(model->modelMat, model->modelMat, tmp);

    mat4x4_translate(tmp, model->worldPos[0], model->worldPos[1], model->worldPos[2]);
    mat4x4_mul(model->modelMat, model->modelMat, tmp);
}

void loadModel(struct Model* model, char path[500]) {

    // Load mesh from file
    fastObjMesh* mesh = fast_obj_read(path);

    model->verticesSize = (int) (mesh->position_count - 1) * 3;
    model->indicesSize = (int) mesh->groups[0].face_count * 3;

    model->vertices = malloc(model->verticesSize + 1);
    model->indices = malloc(model->indicesSize + 1);

    int indicesIndex = 0;

    fastObjGroup grp = mesh->groups[0];
    int idx = 0;
    for (int j = 0; j < mesh->groups[0].face_count; j++) {
        int fv = mesh->face_vertices[grp.face_offset + j];
        for (int k = 0; k < fv; k++) {
            fastObjIndex mi = mesh->indices[grp.index_offset + idx];

            model->indices[indicesIndex] = (mi.p - 1); // Start from 0

            model->vertices[((mi.p - 1) * 3) + 0] = mesh->positions[3 * mi.p + 0];
            model->vertices[((mi.p - 1) * 3) + 1] = mesh->positions[3 * mi.p + 1];
            model->vertices[((mi.p - 1) * 3) + 2] = mesh->positions[3 * mi.p + 2];
            
            idx++;
            indicesIndex++;
        }
    }

    fast_obj_destroy(mesh);



    GL_CALL(glGenVertexArrays(1, &model->VAO));
    GL_CALL(glGenBuffers(1, &model->VBO));
    GL_CALL(glGenBuffers(1, &model->EBO));
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    GL_CALL(glBindVertexArray(model->VAO));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, model->VBO));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, model->verticesSize * 4, model->vertices, GL_STATIC_DRAW));

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->EBO));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->indicesSize * 4, model->indices, GL_STATIC_DRAW));

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GL_CALL(glEnableVertexAttribArray(0));

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0)); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    GL_CALL(glBindVertexArray(0)); 





    defVec3(&model->worldPos, 0.0f, 0.0f, 0.0f);
    defVec3(&model->worldRot, 0.0f, 0.0f, 0.0f);
    model->worldScale = 1.0f;

    reCalcModelMat(model);
}

void freeModel(struct Model* model) {
    GL_CALL(glDeleteVertexArrays(1, &model->VAO));
    GL_CALL(glDeleteBuffers(1, &model->VBO));
    GL_CALL(glDeleteBuffers(1, &model->EBO));

    free(model->vertices);
    free(model->indices);
}