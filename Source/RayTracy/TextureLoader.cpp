#include "TextureLoader.h"
#include <libpng/png.h>
#include <stdio.h>

void ReadImageFileChunk(png_structp pngPtr, png_bytep outBytes, png_size_t byteCountToRead)
{
    auto ioPtr = png_get_io_ptr(pngPtr);
    auto file = (FILE**)ioPtr;
    fread(outBytes, byteCountToRead, 1, *file);
}

Texture TextureLoader::LoadTexture(const std::string& directoryPath, const std::string& path, bool mipmap)
{
    Texture empty(0, 0, 0);
    FILE* file = fopen(path.c_str(), "rb");
    if (!file) {
        auto fullPath = directoryPath + "/" + path;
        file = fopen(fullPath.c_str(), "rb");
        if (!file) {
            return empty;
        }
    }

    unsigned char header[8];
    fread(header, 8, 1, file);
    bool isPng = !png_sig_cmp(header, 0, 8);

    auto pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pngPtr) {
        fclose(file);
        return empty;
    }

    auto pngInfo = png_create_info_struct(pngPtr);
    if (!pngInfo) {
        png_destroy_read_struct(&pngPtr, NULL, NULL);
        fclose(file);
        return empty;
    }

    png_set_read_fn(pngPtr, &file, ReadImageFileChunk);
    png_set_sig_bytes(pngPtr, 8);
    png_read_info(pngPtr, pngInfo);
    png_uint_32 width, height;
    int bitDepth, colorType;
    if (!png_get_IHDR(pngPtr, pngInfo, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL) || !(colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_RGBA)) {
        fclose(file);
        return empty;
    }

    int pixelSize = colorType == PNG_COLOR_TYPE_RGBA ? 4 : 3;
    uint32_t bytesPerRow = pixelSize * width;
    Texture texture(width, height, pixelSize, mipmap);
    auto data = texture.GetData();
    auto rowPointer = data;

    for (uint32_t rowIndex = 0; rowIndex < height; rowIndex++) {
        png_read_row(pngPtr, rowPointer, NULL);
        rowPointer += bytesPerRow;
    }

    fclose(file);
    return texture;
}
