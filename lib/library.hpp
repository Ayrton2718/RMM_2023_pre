#ifndef LIB_LIBRARY_HPP
#define LIB_LIBRARY_HPP

// libs
#include <std_api/std_api.h>
#include <object_parser/object_parser.h>
#include <process_comm/process_comm.h>
#include <collection_class/collection_class.h>

// for c++
#ifdef __cplusplus
#include <std_api/std_api.hpp>
#endif


#ifdef __cplusplus
extern "C" {
#endif

void LIBInitializer_init(const char* server_name, const char* log_path);

#ifdef __cplusplus
}
#endif


#endif /*LIB_LIBRARY_HPP*/