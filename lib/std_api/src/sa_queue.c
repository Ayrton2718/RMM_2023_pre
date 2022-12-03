#include "sa_queue.h"

#include "system/sa_sys_mem.h"

void SAQueue_clear(SAQueue_t* obj)
{
	obj->next_write = 0;
	obj->next_read = 0;
	obj->count = 0;
	memset(obj->ring_buffer, 0x00, obj->data_size * obj->max_count);
}


void SAQueue_init(SAQueue_t* obj, size_t data_size, size_t max_count, void* buffer)
{
	obj->data_size = data_size;

	obj->max_count = max_count;
	obj->ring_buffer = (uint8_t*)buffer;

	SAQueue_clear(obj);
}


size_t SAQueue_count(SAQueue_t* obj)
{
	return obj->count;
}


SABool_t SAQueue_isFill(SAQueue_t* obj)
{
	return obj->count == obj->max_count;
}

size_t SAQueue_maxCount(SAQueue_t* obj)
{
    return obj->max_count;
}


size_t SAQueue_dataSize(SAQueue_t* obj)
{
    return obj->data_size;
}


void SAQueue_add(SAQueue_t* obj, const void* data)
{
	if(SAQueue_isFill(obj))
	{
		obj->count--;
		obj->next_read = (obj->next_read + 1) % obj->max_count;
	}

	memcpy(&obj->ring_buffer[obj->next_write * obj->data_size], data, obj->data_size);
	obj->count++;
    obj->next_write = (obj->next_write + 1) % obj->max_count;
}


void SAQueue_addFront(SAQueue_t* obj, const void* data)
{
	if(SAQueue_isFill(obj))
	{
		obj->count--;
		obj->next_read = (obj->next_read + 1) % obj->max_count;
	}

	obj->next_read = (obj->next_read - 1) % obj->max_count;
	memcpy(&obj->ring_buffer[obj->next_read * obj->data_size], data, obj->data_size);
	obj->count++;
}

void* SAQueue_addFrontRaw(SAQueue_t* obj)
{
	if(SAQueue_isFill(obj))
	{
		obj->count--;
		obj->next_read = (obj->next_read + 1) % obj->max_count;
	}

	obj->next_read = (obj->next_read - 1) % obj->max_count;
	obj->count++;
	return &obj->ring_buffer[obj->next_read * obj->data_size];
}


void* SAQueue_addRaw(SAQueue_t* obj)
{
	if(SAQueue_isFill(obj))
	{
		obj->count--;
		obj->next_read = (obj->next_read + 1) % obj->max_count;
	}

	size_t befo_next_write = obj->next_write;
	obj->count++;
    obj->next_write = (obj->next_write + 1) % obj->max_count;
	return &obj->ring_buffer[befo_next_write * obj->data_size];
}


int SAQueue_readonly(SAQueue_t* obj, void* data, size_t index)
{
	if(obj->count <= index)
	{
		return 0;
	}else{
        index = (obj->next_read + index) % obj->max_count;
		memcpy(data, &obj->ring_buffer[obj->next_read * obj->data_size], obj->data_size);
		return 1;
	}
}

int SAQueue_get(SAQueue_t* obj, void* data)
{
	if(obj->count == 0)
	{
		return 0;
	}else{
		memcpy(data, &obj->ring_buffer[obj->next_read * obj->data_size], obj->data_size);
		obj->count--;
        obj->next_read = (obj->next_read + 1) % obj->max_count;
		return 1;
	}
}

void* SAQueue_getRaw(SAQueue_t* obj)
{
	if(obj->count == 0)
	{
		return NULL;
	}else{
		size_t befo_next_read = obj->next_read;
		obj->count--;
        obj->next_read = (obj->next_read + 1) % obj->max_count;
		return &obj->ring_buffer[befo_next_read * obj->data_size];
	}
}