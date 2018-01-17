#include "MeshLoader.h"
#include <stdio.h>
#include <vector>
#include <string.h>
#include <cstdlib>

const uint32_t MeshLoader::LineLength = 200;
const uint32_t MeshLoader::TokenLength = 200;
const char* MeshLoader::Delimiters = " /";

bool MeshLoader::ParseFloat(float& value)
{
    auto token = strtok(0, Delimiters);
    if (!token) {
        return false;
    }
    value = atof(token);
    return true;
}

bool MeshLoader::ParseInt(int& value)
{
    auto token = strtok(0, Delimiters);
    if (!token) {
        return false;
    }
    value = atoi(token);
    return true;
}

bool MeshLoader::LoadMesh(const std::string& directoryPath, const std::string& path, Mesh* mesh)
{
    FILE* file = fopen(path.c_str(), "r");
    if (!file) {
        auto fullPath = directoryPath + "/" + path;
        file = fopen(fullPath.c_str(), "r");
        if (!file) {
            return false;
        }
    }

    char line[LineLength];
    std::vector<Vector3> vertices;
    std::vector<Vector2> textureCoordinates;
    std::vector<int> faces;

    while (!feof(file)) {
        line[0] = 0;
        fgets(line, LineLength, file);
        if (line[0] == 0) {
            break;
        }
        auto name = strtok(line, Delimiters);
        if (strcmp(name, "v") == 0) {
            Vector3 vertex;
            if (!ParseFloat(vertex.x) || !ParseFloat(vertex.y) || !ParseFloat(vertex.z)) {
                return false;
            }
            vertices.push_back(vertex);
        }
        else if (strcmp(name, "vt") == 0) {
            Vector2 textureCoordinate;
            if (!ParseFloat(textureCoordinate.x) || !ParseFloat(textureCoordinate.y)) {
                return false;
            }
            textureCoordinates.push_back(textureCoordinate);
        }
        else if (strcmp(name, "f") == 0) {
            int index;
            for (uint32_t i = 0; i < 6; i++) {
                if (!ParseInt(index) || index <= 0) {
                    return false;
                }
                faces.push_back(index);
            }
        }
    }

    mesh->Resize(textureCoordinates.size(), faces.size() / 2, true);

    for (uint32_t i = 0; i < textureCoordinates.size(); i++) {
        mesh->SetTextureCoordinate(i, textureCoordinates[i]);
    }

    for (uint32_t i = 0; i < faces.size(); i += 6) {
        for (uint32_t j = 0; j < 6; j += 2) {
            uint32_t vertexIndex = faces[i + j] - 1;
            uint32_t textureCoordinateIndex = faces[i + j + 1] - 1;
            mesh->SetVertex(textureCoordinateIndex, vertices[vertexIndex]);
            mesh->SetIndex((i + j) / 2, textureCoordinateIndex);
        }
    }

    fclose(file);
    return true;
}
