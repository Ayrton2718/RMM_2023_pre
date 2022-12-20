#include "wm_gl.h"

#include <arpa/inet.h>
#include <object_parser/object_parser.h>

typedef enum
{
    WMGl_objectType_Line = 0,
    WMGl_objectType_Circle = 1,
    WMGl_objectType_Polygon = 2,
} WMGl_objectType_t;

typedef enum
{
    WMGl_jsonType_Reset = 0,
    WMGl_jsonType_Preload = 1,
    WMGl_jsonType_Layer = 2,
} WMGl_jsonType_t;


typedef struct
{
    int sock;
    struct sockaddr_in send_addr;

    float width;
    float height;
} WMGl_info_t;


WMGl_t WMGl_create(const char* ip, uint16_t port, float width, float height)
{
    WMGl_info_t* _obj = SAMEM_MALLOC(sizeof(WMGl_info_t));
    memset(_obj, 0x00, sizeof(WMGl_info_t));

    _obj->sock = SASocket_socket(AF_INET, SOCK_DGRAM, 0);
    
    _obj->send_addr.sin_family = AF_INET;
    _obj->send_addr.sin_port = htons(port);
    _obj->send_addr.sin_addr.s_addr = inet_addr(ip);

    _obj->width = width;
    _obj->height = height;


    CCAutoRelease_startScope();
    {
        CC_obj send_obj = CCAutoRelease_add(CCDictionary_create());
        CCDictionary_setObject(send_obj, CCInt32_create(WMGl_jsonType_Reset), "jsonType");
        CC_obj send_json = CCAutoRelease_add(CCJsonSerializer_dump(send_obj, SABOOL_FALSE));
        const char* send_str = (const char*)CCBinary_getRaw(send_json);
        size_t send_len = CCBinary_size(send_json);
        SASocket_sendto(_obj->sock, send_str, send_len, 0, (struct sockaddr*)&_obj->send_addr, sizeof(struct sockaddr_in));
        SALOG_INFO("sdfs", "%d, %s", send_len, send_str);

        SATime_delaySec(1);

        send_obj = CCAutoRelease_add(CCDictionary_create());
        CCDictionary_setObject(send_obj, CCInt32_create(WMGl_jsonType_Preload), "jsonType");
        CC_obj size = CCArray_create();
        CCArray_addObject(size, CCDouble_create(_obj->width));
        CCArray_addObject(size, CCDouble_create(_obj->height));
        CCDictionary_setObject(send_obj, size, "size");
        send_json = CCAutoRelease_add(CCJsonSerializer_dump(send_obj, SABOOL_FALSE));
        send_str = (const char*)CCBinary_getRaw(send_json);
        send_len = CCBinary_size(send_json);
        SASocket_sendto(_obj->sock, send_str, send_len, 0, (struct sockaddr*)&_obj->send_addr, sizeof(struct sockaddr_in));
        SALOG_INFO("sdfs", "%d, %s", send_len, send_str);
    }
    CCAutoRelease_doneScope();

    return _obj;
}



WMGl_layer_t WMGl_createLayer(WMGl_t obj)
{
    return CCArray_create();
}


void WMGl_addLine(WMGl_layer_t layer, WMGl_color_t col, const WMGl_vector_t* vertex, size_t count)
{
    CC_obj object = CCDictionary_create();

    CCDictionary_setObject(object, CCInt32_create(WMGl_objectType_Line), "type");
    
    CC_obj col_array = CCArray_create();
    CCArray_addObject(col_array, CCInt32_create(col.r));
    CCArray_addObject(col_array, CCInt32_create(col.g));
    CCArray_addObject(col_array, CCInt32_create(col.b));
    CCDictionary_setObject(object, col_array, "color");

    CC_obj vertex_array = CCArray_create();
    for(size_t i = 0; i < count; i++)
    {
        CCArray_addObject(vertex_array, CCDouble_create(vertex[i].vx));
        CCArray_addObject(vertex_array, CCDouble_create(vertex[i].vy));
    }
    CCDictionary_setObject(object, vertex_array, "vertex");

    CCArray_addObject(layer, object);
}

void WMGl_addCircle(WMGl_layer_t layer, WMGl_color_t col, WMGl_vector_t center, float radius)
{
    CC_obj object = CCDictionary_create();

    CCDictionary_setObject(object, CCInt32_create(WMGl_objectType_Circle), "type");
    
    CC_obj col_array = CCArray_create();
    CCArray_addObject(col_array, CCInt32_create(col.r));
    CCArray_addObject(col_array, CCInt32_create(col.g));
    CCArray_addObject(col_array, CCInt32_create(col.b));
    CCDictionary_setObject(object, col_array, "color");

    CC_obj center_array = CCArray_create();
    CCArray_addObject(center_array, CCDouble_create(center.vx));
    CCArray_addObject(center_array, CCDouble_create(center.vy));
    CCDictionary_setObject(object, center_array, "center");

    CCDictionary_setObject(object, CCDouble_create(radius), "radius");

    CCArray_addObject(layer, object);
}

void WMGl_addPolygon(WMGl_layer_t layer, WMGl_color_t col, const WMGl_vector_t* vertex, size_t count, float rate)
{
    CC_obj object = CCDictionary_create();

    CCDictionary_setObject(object, CCInt32_create(WMGl_objectType_Polygon), "type");

    CC_obj col_array = CCArray_create();
    CCArray_addObject(col_array, CCInt32_create(col.r));
    CCArray_addObject(col_array, CCInt32_create(col.g));
    CCArray_addObject(col_array, CCInt32_create(col.b));
    CCDictionary_setObject(object, col_array, "color");
    
    CC_obj vertex_array = CCArray_create();
    for(size_t i = 0; i < count; i++)
    {
        CCArray_addObject(vertex_array, CCDouble_create(vertex[i].vx));
        CCArray_addObject(vertex_array, CCDouble_create(vertex[i].vy));
    }
    CCDictionary_setObject(object, vertex_array, "vertex");

    CCArray_addObject(layer, object);
}


void WMGl_render(WMGl_t obj, size_t layer_num, WMGl_layer_t layer)
{
    WMGl_info_t* _obj = (WMGl_info_t*)obj;

    CCAutoRelease_startScope();
    {
        CC_obj send_obj = CCAutoRelease_add(CCDictionary_create());
        CCDictionary_setObject(send_obj, CCInt32_create(WMGl_jsonType_Layer), "jsonType");
        CCDictionary_setObject(send_obj, CCInt32_create(layer_num), "layer");
        CCDictionary_setObject(send_obj, layer, "object");

        CC_obj send_json = CCAutoRelease_add(CCJsonSerializer_dump(send_obj, SABOOL_FALSE));
        const char* send_str = (const char*)CCBinary_getRaw(send_json);
        size_t send_len = CCBinary_size(send_json);
        SASocket_sendto(_obj->sock, send_str, send_len, 0, (struct sockaddr*)&_obj->send_addr, sizeof(struct sockaddr_in));

        SALOG_INFO("sdfs", "%d, %s", send_len, send_str);
    }
    CCAutoRelease_doneScope();
}
