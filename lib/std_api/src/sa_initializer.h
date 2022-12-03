#ifndef STD_API_SRC_SA_INITIALIZER_H
#define STD_API_SRC_SA_INITIALIZER_H

#include "system/sa_type.h"
#include <stdio.h>

#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

void SAInitializer_init(const char* log_path);

#ifdef __cplusplus
}
#endif

#endif /*STD_API_SRC_SA_INITIALIZER_H*/