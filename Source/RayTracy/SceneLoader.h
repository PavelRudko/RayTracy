#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "Scene.h"
#include <stdio.h>

class SceneLoader
{
private:
    static const uint32_t LineLength;
    static const uint32_t TokenLength;
    static const char* Delimiters;

    bool IsEmptyLine(const char* line);
    bool ParseFloat(float& value);
    bool ParseInt(int& value);
    bool ParseVector(Vector3& vector);
    bool ParseScene(FILE* file, Scene* scene, uint32_t& lineNumber, char* line, char* token);
    bool ParseSphere(FILE* file, Sphere* sphere, uint32_t& lineNumber, char* line, char* token);
    bool ParsePlane(FILE* file, Plane* plane, uint32_t& lineNumber, char* line, char* token);
    bool ParseDisk(FILE* file, Disk* disk, uint32_t& lineNumber, char* line, char* token);
    bool ParseTriangle(FILE* file, Triangle* triangle, uint32_t& lineNumber, char* line, char* token);
    bool ParseMesh(FILE* file, Mesh* mesh, uint32_t& lineNumber, char* line, char* token);
    bool ParseLight(FILE* file, Light* light, uint32_t& lineNumber, char* line, char* token);
    bool ParseTexture(FILE* file, Scene* scene, uint32_t& lineNumber, char* line, char* token);
    bool ParseFile(FILE* file, Scene* scene);

public:
    bool LoadScene(const char* path, Scene* scene);
};

#endif