#include "File.hpp"

#include <cassert>
#include <cstdlib>

long countFileSize(std::FILE *file)
{
    assert(file != nullptr);

    fseek(file, 0, SEEK_END);

    long fileSize = ftell(file);
    rewind(file);

    return fileSize;
}

char *readFileToBuf(FILE *file)
{
    assert(file != nullptr);

    long fileSize = countFileSize(file);
    auto buffer = (char *) std::calloc(fileSize, sizeof(char));
    fread(buffer, sizeof(*buffer), fileSize, file);

    return buffer;
}
