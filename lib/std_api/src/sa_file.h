#ifndef STD_API_SRC_SA_FILE_H
#define STD_API_SRC_SA_FILE_H

#include "system/sa_type.h"
#include <stdio.h>

#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

FILE* SAFile_fopen(const char *pathname, const char *mode);
void SAFile_fclose(FILE* stream);

void SAFile_setlinebuf(FILE* stream);

void SAFile_mkdir(const char* pathname, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /*STD_API_SRC_SA_FILE_H*/