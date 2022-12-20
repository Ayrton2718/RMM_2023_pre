#include "cc_type.h"

struct CCTypeName_t{
    CC_type_t id;
    const char* name;
};

struct CCTypeName_t g_type_ids[] = {
    {CC_NULL_TYPE_ID, "Null"},
    {CC_BOOLEAN_TYPE_ID, "Boolean"},
    {CC_INT64_TYPE_ID, "Int64"},
    {CC_INT32_TYPE_ID, "Int32"},
    {CC_INT16_TYPE_ID, "Int16"},
    {CC_VARIABLE_TYPE_ID, "Variable"},
    {CC_BYTES_TYPE_ID, "Bytes"},
    {CC_FLOAT_TYPE_ID, "Float"},
    {CC_DOUBLE_TYPE_ID, "Double"},
    {CC_STRING_TYPE_ID, "String"},
    {CC_BINARY_TYPE_ID, "Binary"},
    {CC_STRUCT_TYPE_ID, "Struct"},
    {CC_DICTIONARY_TYPE_ID, "Dictionary"},
    {CC_ARRAY_TYPE_ID, "Array"},
    {CC_SET_TYPE_ID, "Set"},
    {CC_QUEUE_TYPE_ID, "Queue"},
};

const char* CCType_toName(CC_type_t type_id)
{
    for(size_t i = 0; i < (sizeof(g_type_ids) / sizeof(struct CCTypeName_t)); i++)
    {
        if(type_id == g_type_ids[i].id)
        {
            return g_type_ids[i].name;
        }
    }

    return "Unknown";
}
