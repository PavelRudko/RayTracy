#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include "Geometry.h"
#include <string>

class MeshLoader
{
private:
    static const uint32_t LineLength;
    static const uint32_t TokenLength;
    static const char* Delimiters;

    bool ParseFloat(float& value);
    bool ParseInt(int& value);

public:
    bool LoadMesh(const std::string& directoryPath, const std::string& path, Mesh* mesh);
};

#endif