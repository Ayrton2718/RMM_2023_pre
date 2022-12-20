#ifndef LRF_LOCALIZATION_SRC_WM_GL_H
#define LRF_LOCALIZATION_SRC_WM_GL_H

#include <std_api/std_api.h>
#include <collection_class/collection_class.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} WMGl_color_t;

typedef struct
{
    float vx;
    float vy;
} WMGl_vector_t;

typedef void* WMObject_t;
typedef CC_obj WMGl_layer_t;

typedef void* WMGl_t;

WMGl_t WMGl_create(const char* ip, uint16_t port, float width, float height);

WMGl_layer_t WMGl_createLayer(WMGl_t obj);
void WMGl_addLine(WMGl_layer_t layer, WMGl_color_t col, const WMGl_vector_t* vertex, size_t count);
void WMGl_addCircle(WMGl_layer_t layer, WMGl_color_t col, WMGl_vector_t center, float radius);
void WMGl_addPolygon(WMGl_layer_t layer, WMGl_color_t col, const WMGl_vector_t* vertex, size_t count, float rate);

void WMGl_render(WMGl_t obj, size_t layer_num, WMGl_layer_t layer);

CC_obj WMGl_getKey(WMGl_t obj, uint32_t timeout_ms); 

#ifdef __cplusplus
}
#endif

#endif /*LRF_LOCALIZATION_SRC_WM_GL_H*/
