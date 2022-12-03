#ifndef STD_API_SRC_SA_QUEUE_H
#define STD_API_SRC_SA_QUEUE_H

#include "system/sa_type.h"
#include "sa_bool.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
	size_t next_write;
	size_t next_read;
	size_t count;
	size_t max_count;

	size_t data_size;
	uint8_t* ring_buffer;
} SAQueue_t;


void SAQueue_clear(SAQueue_t* obj);


void SAQueue_init(SAQueue_t* obj, size_t data_size, size_t max_count, void* buffer);

size_t SAQueue_count(SAQueue_t* obj);


SABool_t SAQueue_isFill(SAQueue_t* obj);

size_t SAQueue_maxCount(SAQueue_t* obj);


size_t SAQueue_dataSize(SAQueue_t* obj);


void SAQueue_add(SAQueue_t* obj, const void* data);


void SAQueue_addFront(SAQueue_t* obj, const void* data);

void* SAQueue_addFrontRaw(SAQueue_t* obj);


void* SAQueue_addRaw(SAQueue_t* obj);


int SAQueue_readonly(SAQueue_t* obj, void* data, size_t index);

int SAQueue_get(SAQueue_t* obj, void* data);

void* SAQueue_getRaw(SAQueue_t* obj);

#ifdef __cplusplus
}
#endif

#endif /*STD_API_SRC_SA_QUEUE_H*/
