#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "Texture.h"
#include <string>

class TextureLoader
{
private:
public:
    Texture LoadTexture(const std::string& directoryPath, const std::string& path, bool mipmap);
};

#endif