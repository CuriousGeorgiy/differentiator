#ifndef FILE_HPP
#define FILE_HPP

#include <cstdio>

long countFileSize(std::FILE *file);

char *readFileToBuf(FILE *file);

#endif /* FILE_HPP */
