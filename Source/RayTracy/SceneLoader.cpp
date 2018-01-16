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

bool SceneLoader::ParseBool(bool& value)
{
    auto token = strtok(0, Delimiters);
    if (!token) {
        return false;
    }
    value = strcmp(token, "true") == 0;
    if (!value && !(strcmp(token, "false") == 0)) {
        return false;
    }
    return true;
}

std::string SceneLoader::ParseString()
{
    auto token = strtok(0, Delimiters);
    std::string value(token);
    uint32_t valueLength = value.find_last_of(" \n\r");
    return value.substr(0, valueLength);
}

bool SceneLoader::ParseVector(Vector3& vector) {
    return ParseFloat(vector.x) && ParseFloat(vector.y) && ParseFloat(vector.z);
}

#define LookForInt(fieldName, field) if(strcmp(name, fieldName) == 0) { result = ParseInt(field); }
#define LookForVector(fieldName, field) if(strcmp(name, fieldName) == 0) { result = ParseVector(field); }
#define LookForFloat(fieldName, field) if(strcmp(name, fieldName) == 0) { result = ParseFloat(field); }
#define LookForBool(fieldName, field) if(strcmp(name, fieldName) == 0) { result = ParseBool(field); }

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

#define LookForMaterial(material)                               \
    LookForVector("color", material.color);                     \
    LookForFloat("ka", material.Ka);                            \
    LookForFloat("kd", material.Kd);                            \
    LookForFloat("ks", material.Ks);                            \
    LookForFloat("spow", material.S);                           \
    LookForFloat("textureScale", material.textureScale);        \
    LookForFloat("reflectivity", material.reflectivity);        \
    LookForFloat("ior", material.ior);                          \
    LookForFloat("mipBias", material.mipBias);                  \
    LookForInt("texture", material.texture);

#define RequireInt(fieldName, description, field)                               \
    fgets(line, LineLength, file);                                              \
    lineNumber++;                                                               \
    if (strcmp(strtok(line, Delimiters), fieldName) != 0 || !ParseInt(field)) { \
        Missing(description, lineNumber);                                       \
        return false;                                                           \
    }

bool SceneLoader::ParseScene(FILE* file, Scene* scene, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Scene",
        LookForVector("backgroundColor", scene->backgroundColor);
    )
}

bool SceneLoader::ParseSphere(FILE* file, Sphere* sphere, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Sphere",
        LookForVector("center", sphere->center);
        LookForFloat("radius", sphere->radius);
        LookForMaterial(sphere->material)
    )
}

bool SceneLoader::ParsePlane(FILE* file, Plane* plane, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Plane",
        LookForVector("point", plane->point);
        LookForVector("normal", plane->normal);
        LookForMaterial(plane->material)
    )
}

bool SceneLoader::ParseDisk(FILE* file, Disk* disk, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Disk",
        LookForVector("point", disk->point);
        LookForVector("normal", disk->normal);
        LookForFloat("radius", disk->radius);
        LookForMaterial(disk->material)
    )
}

bool SceneLoader::ParseTriangle(FILE* file, Triangle* triangle, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Triangle",
        LookForVector("a", triangle->a);
        LookForVector("b", triangle->b);
        LookForVector("c", triangle->c);
        LookForMaterial(triangle->material)
    )
}

bool SceneLoader::ParseMesh(FILE* file, Mesh* mesh, uint32_t& lineNumber, char* line, char* token)
{
    int verticesCount, indicesCount;

    RequireInt("vertices", "Vertices count", verticesCount);
    RequireInt("indices", "Indices count", indicesCount);

    mesh->Resize(verticesCount, indicesCount);

    for (int i = 0; i < verticesCount; i++) {
        fgets(line, LineLength, file);
        lineNumber++;
        Vector3 vertex;
        if (sscanf(line, "%f,%f,%f", &vertex.x, &vertex.y, &vertex.z) != 3) {
            Missing("Vertices", lineNumber);
            return false;
        }
        mesh->SetVertex(i, vertex);
    }

    for (int i = 0; i < indicesCount; i++) {
        fgets(line, LineLength, file);
        lineNumber++;
        uint32_t index;
        if (sscanf(line, "%d", &index) != 1) {
            Missing("Indices", lineNumber);
            return false;
        }
        if (index < 0 || index >= verticesCount) {
            printf("Index is out of range at line %d.\n", lineNumber);
            return false;
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

        auto name = strtok(line, Delimiters);
        LookForMaterial(mesh->material)
    }

    if (!result) {
        CannotParse("Mesh", lineNumber);
        return false;
    }

    return true;
}

bool SceneLoader::ParseLight(FILE* file, Light* light, uint32_t& lineNumber, char* line, char* token)
{
    LineByLine
    (
        "Light",
        LookForVector("position", light->position);
        LookForVector("color", light->color);
    )
}

bool SceneLoader::ParseTexture(FILE * file, Scene * scene, uint32_t& lineNumber, char* line, char* token, const std::string& directoryPath)
{
    int width = -1, height = -1, bytesPerPixel = -1, mipmap = 0;

    bool result = true;
    while (!feof(file) && result) {
        fgets(line, LineLength, file);                          
        lineNumber++;                                           
        if (IsEmptyLine(line)) {
            break;                                              
        }                                                       
        auto name = strtok(line, Delimiters);
        LookForInt("width", width);
        LookForInt("height", height);
        LookForInt("bytesPerPixel", bytesPerPixel);
        LookForInt("mipmap", mipmap);
        if (strcmp("pixels", name) == 0) {
            break;
        }
        if (strcmp("path", name) == 0) {
            auto path = ParseString();
            scene->textures.push_back(textureLoader.LoadTexture(directoryPath, path, mipmap));
            Texture& texture = scene->textures[scene->textures.size() - 1];
            if (texture.GetHeight() == 0 || texture.GetWidth() == 0) {
                printf("Cannot load texture file %s.\n", path.c_str());
                return false;
            }
            if (mipmap == 1) {
                texture.GenerateMipmap();
            }
            return true;
        }
    }

    if (width < 0 || height < 0 || bytesPerPixel < 3 || bytesPerPixel > 4 || (mipmap != 0 && mipmap != 1)) {
        printf("Unacceptable values for texture at line %d.\n", lineNumber);
        return false;
    }

    scene->textures.push_back(Texture(width, height, bytesPerPixel, mipmap == 1));
    Texture& texture = scene->textures[scene->textures.size() - 1];

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            fgets(line, LineLength, file);
            lineNumber++;
            int r, g, b, a = 1;
            if (sscanf(line, bytesPerPixel == 3 ? "%d,%d,%d" : "%d,%d,%d,%d", &r, &g, &b, &a) != bytesPerPixel) {
                printf("Cannot parse texture color at line %d.\n", lineNumber);
                return false;
            }
            texture.SetPixel(x, y, r, g, b, a);
        }
    }

    if (mipmap == 1) {
        texture.GenerateMipmap();
    }

    return true;
}

bool SceneLoader::ParseFile(FILE* file, Scene* scene, const std::string& directoryPath)
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
            auto sphere = new Sphere();
            result = ParseSphere(file, sphere, lineNumber, line, token);
            scene->objects.push_back(std::unique_ptr<Object>(sphere));
        }
        else if (strcmp(token, "Plane") == 0) {
            auto plane = new Plane;
            result = ParsePlane(file, plane, lineNumber, line, token);
            plane->normal.Normalize();
            scene->objects.push_back(std::unique_ptr<Object>(plane));
        }
        else if (strcmp(token, "Disk") == 0) {
            auto disk = new Disk;
            result = ParseDisk(file, disk, lineNumber, line, token);
            disk->normal.Normalize();
            scene->objects.push_back(std::unique_ptr<Object>(disk));
        }
        else if (strcmp(token, "Triangle") == 0) {
            auto triangle = new Triangle;
            result = ParseTriangle(file, triangle, lineNumber, line, token);
            scene->objects.push_back(std::unique_ptr<Object>(triangle));
        }
        else if (strcmp(token, "Light") == 0) {
            Light light;
            result = ParseLight(file, &light, lineNumber, line, token);
            scene->lights.push_back(light);
        }
        else if (strcmp(token, "Mesh") == 0) {
            auto mesh = new Mesh;
            result = ParseMesh(file, mesh, lineNumber, line, token);
            scene->objects.push_back(std::unique_ptr<Object>(mesh));
        }
        else if (strcmp(token, "Texture") == 0) {
            result = ParseTexture(file, scene, lineNumber, line, token, directoryPath);
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

bool SceneLoader::LoadScene(const char* path, Scene* scene)
{
    scene->backgroundColor = Vector3{ 0, 0, 0 };
    auto directoryPath = GetDirectoryPath(path);

    auto file = fopen(path, "r");
    if (!file) {
        printf("File is not found.\n");
        return false;
    }
    
    bool result = ParseFile(file, scene, directoryPath);
    fclose(file);

    if (!result) {
        scene->objects.clear();
        scene->lights.clear();
    }

    return result;
}

std::string SceneLoader::GetDirectoryPath(const char* path) const
{
    std::string str(path);
    uint32_t directoryPathLength = str.find_last_of("/\\");
    return str.substr(0, directoryPathLength);
}
