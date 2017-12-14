#include "SceneLoader.h"
#include <string.h>
#include <cstdlib>

const uint32_t SceneLoader::LineLength = 200;
const uint32_t SceneLoader::TokenLength = 200;
const char* SceneLoader::Delimiters = " \n,:";

inline bool CannotParse(const char* name, uint32_t lineNumber)
{
    printf("Cannot parse %s at line %d.\n", name, lineNumber);
    return false;
}

inline bool Missing(const char* name, uint32_t lineNumber)
{
    printf("%s missing at line %d.\n", name, lineNumber);
    return false;
}

bool SceneLoader::IsEmptyLine(const char* line)
{
    char c;
    for (uint32_t i = 0; (c = line[i]) != '\0'; i++) {
        if (c != '\n' && c != ' ' && c != '\r') {
            return false;
        }
    }
    return true;
}

bool SceneLoader::ParseFloat(float& value)
{
    auto token = strtok(0, Delimiters);
    if (!token) {
        return false;
    }
    value = atof(token);
    return true;
}

bool SceneLoader::ParseInt(int& value)
{
    auto token = strtok(0, Delimiters);
    if (!token) {
        return false;
    }
    value = atoi(token);
    return true;
}

bool SceneLoader::ParseVector(Vector3& vector) {
    return ParseFloat(vector.x) && ParseFloat(vector.y) && ParseFloat(vector.z);
}

#define LookForVector(fieldName, field) if(strcmp(name, fieldName) == 0) { result = ParseVector(field); }
#define LookForFloat(fieldName, field) if(strcmp(name, fieldName) == 0) { result = ParseFloat(field); }
#define LineByLine(objectName, code)                            \
    bool result = true;                                         \
    while(!feof(file) && result){                               \
        fgets(line, LineLength, file);                          \
        lineNumber++;                                           \
        if(IsEmptyLine(line)){                                  \
            break;                                              \
        }                                                       \
        auto name = strtok(line, Delimiters);                   \
        code                                                    \
    }                                                           \
    return result ? true : CannotParse(objectName, lineNumber);


bool SceneLoader::ParseScene(FILE* file, Scene& scene, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Scene",
        LookForVector("backgroundColor", scene.backgroundColor);
    )
}

bool SceneLoader::ParseSphere(FILE* file, Sphere& sphere, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Sphere",
        LookForVector("center", sphere.center);
        LookForFloat("radius", sphere.radius);
        LookForVector("color", sphere.material.color);
        LookForFloat("ka", sphere.material.Ka);
        LookForFloat("kd", sphere.material.Kd);
        LookForFloat("ks", sphere.material.Ks);
        LookForFloat("spow", sphere.material.S);
    )
}

bool SceneLoader::ParsePlane(FILE* file, Plane& plane, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Plane",
        LookForVector("point", plane.point);
        LookForVector("normal", plane.normal);
        LookForVector("color", plane.material.color);
        LookForFloat("ka", plane.material.Ka);
        LookForFloat("kd", plane.material.Kd);
        LookForFloat("ks", plane.material.Ks);
        LookForFloat("spow", plane.material.S);
    )
}

bool SceneLoader::ParseDisk(FILE* file, Disk& disk, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Disk",
        LookForVector("point", disk.point);
        LookForVector("normal", disk.normal);
        LookForFloat("radius", disk.radius);
        LookForVector("color", disk.material.color);
        LookForFloat("ka", disk.material.Ka);
        LookForFloat("kd", disk.material.Kd);
        LookForFloat("ks", disk.material.Ks);
        LookForFloat("spow", disk.material.S);
    )
}

bool SceneLoader::ParseTriangle(FILE* file, Triangle& triangle, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Triangle",
        LookForVector("a", triangle.a);
        LookForVector("b", triangle.b);
        LookForVector("c", triangle.c);
        LookForVector("color", triangle.material.color);
        LookForFloat("ka", triangle.material.Ka);
        LookForFloat("kd", triangle.material.Kd);
        LookForFloat("ks", triangle.material.Ks);
        LookForFloat("spow", triangle.material.S);
    )
}

Mesh* SceneLoader::ParseMesh(FILE* file, uint32_t& lineNumber, char* line, char* token)
{
    int verticesCount, indicesCount;

    fgets(line, LineLength, file);
    lineNumber++;
    auto name = strtok(line, Delimiters);
    if (strcmp(name, "vertices") != 0 || !ParseInt(verticesCount)) {
        Missing("Vertices count", lineNumber);
        return nullptr;
    }

    fgets(line, LineLength, file);
    lineNumber++;
    name = strtok(line, Delimiters);
    if (strcmp(name, "indices") != 0 || !ParseInt(indicesCount)) {
        Missing("Indices count", lineNumber);
        return nullptr;
    }

    auto mesh = new Mesh(verticesCount, indicesCount);

    for (int i = 0; i < verticesCount; i++) {
        fgets(line, LineLength, file);
        lineNumber++;
        Vector3 vertex;
        if (sscanf(line, "%f,%f,%f", &vertex.x, &vertex.y, &vertex.z) != 3) {
            Missing("Vertices", lineNumber);
            delete mesh;
            return nullptr;
        }
        mesh->SetVertex(i, vertex);
    }

    for (int i = 0; i < indicesCount; i++) {
        fgets(line, LineLength, file);
        lineNumber++;
        uint32_t index;
        if (sscanf(line, "%d", &index) != 1) {
            Missing("Indices", lineNumber);
            delete mesh;
            return nullptr;
        }
        if (index < 0 || index >= verticesCount) {
            printf("Index is out of range at line %d.\n", lineNumber);
            delete mesh;
            return nullptr;
        }
        mesh->SetIndex(i, index);
    }

    bool result = true;
    while (!feof(file) && result) {
        fgets(line, LineLength, file);
        lineNumber++;

        if (IsEmptyLine(line)) {
            break;
        }

        name = strtok(line, Delimiters);
        LookForVector("color", mesh->material.color);
        LookForFloat("ka", mesh->material.Ka);
        LookForFloat("kd", mesh->material.Kd);
        LookForFloat("ks", mesh->material.Ks);
        LookForFloat("spow", mesh->material.S);
    }

    if (!result) {
        CannotParse("Mesh", lineNumber);
        delete mesh;
        return nullptr;
    }

    return mesh;
}

bool SceneLoader::ParseLight(FILE* file, Light& light, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Light",
        LookForVector("position", light.position);
        LookForVector("color", light.color);
    )
}

bool SceneLoader::ParseFile(FILE* file, Scene& scene)
{
    uint32_t lineNumber = 0;
    char line[LineLength], token[TokenLength];

    while (!feof(file)) {
        fgets(line, LineLength, file);
        lineNumber++;

        if (IsEmptyLine(line)) {
            continue;
        }

        sscanf(line, "%s", token);
        
        bool result;
        if (strcmp(token, "Scene") == 0) {
            result = ParseScene(file, scene, lineNumber, line, token);
        }
        else if (strcmp(token, "Sphere") == 0) {
            auto sphere = new Sphere;
            result = ParseSphere(file, *sphere, lineNumber, line, token);
            scene.objects.push_back(sphere);
        }
        else if (strcmp(token, "Plane") == 0) {
            auto plane = new Plane;
            result = ParsePlane(file, *plane, lineNumber, line, token);
            plane->normal.Normalize();
            scene.objects.push_back(plane);
        }
        else if (strcmp(token, "Disk") == 0) {
            auto disk = new Disk;
            result = ParseDisk(file, *disk, lineNumber, line, token);
            disk->normal.Normalize();
            scene.objects.push_back(disk);
        }
        else if (strcmp(token, "Triangle") == 0) {
            auto triangle = new Triangle;
            result = ParseTriangle(file, *triangle, lineNumber, line, token);
            scene.objects.push_back(triangle);
        }
        else if (strcmp(token, "Light") == 0) {
            Light light;
            result = ParseLight(file, light, lineNumber, line, token);
            scene.lights.push_back(light);
        }
        else if (strcmp(token, "Mesh") == 0) {
            auto mesh = ParseMesh(file, lineNumber, line, token);
            result = mesh != nullptr;
            if (mesh) {
                scene.objects.push_back(mesh);
            }
        }
        else {
            printf("Unknown token '%s' at line %d.\n", token, lineNumber);
            return false;
        }

        if (!result) {
            return false;
        }
    }

    return true;
}

bool SceneLoader::LoadScene(const char* path, Scene& scene)
{
    scene.backgroundColor = Vector3{ 0, 0, 0 };

    auto file = fopen(path, "r");
    if (!file) {
        printf("File is not found.\n");
        return false;
    }

    bool result = ParseFile(file, scene);
    fclose(file);

    if (!result) {
        for (auto object : scene.objects) {
            delete object;
        }
        scene.objects.clear();
        scene.lights.clear();
    }

    return result;
}
