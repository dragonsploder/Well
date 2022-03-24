#include "well.h"

#ifdef DEBUG
    void glClearErrors(){
        while(glGetError() != GL_NO_ERROR);
    }

    void glCheckError(const char* file, int line, const char* function){
        unsigned int error;
        error = glGetError();
        if (error) {
            DEBUG_LOG("File:%s, Line:%i\n", file, line);
            DEBUG_LOG("    Function:%s\n", function);
            DEBUG_LOG("        Error:%u (0x%x)\n", error, error);
            exit(0);
        }
    }
#endif


void defVec3(vec3* vec, float x, float y, float z) {
    DEBUG_LOG("Define vec3 x:%f, y:%f, z:%f", x, y, z);
    (*vec)[0] = x;
    (*vec)[1] = y;
    (*vec)[2] = z;
}


bool readFile(char path[200], char** fileData, size_t* size) {
    DEBUG_LOG("Read file:%s", path);
    FILE* file;
    file = fopen(path, "rb");

    fseek(file, 0L, SEEK_END);
    (*size) = ftell(file);
    (*fileData) = malloc(*size + 1);
    rewind(file);

    if (file) {
        fread((*fileData), *size, 1, file);
        fclose(file);
    } else {
        printf("Cant read file:%s", path);
        return false;
    }

    (*fileData)[(*size)] = '\0';

    return true;
}