#include "jsmn_interface.h"


static struct JsmnInterface_token_t JsmnInterface_searchTokCurRange(struct JsmnInterface_t* obj, size_t tokenIndex)
{
    struct JsmnInterface_token_t range = {tokenIndex, obj->parser.toknext-tokenIndex};
    int parent = obj->parser.tokens[tokenIndex].parent;

    for( size_t i=1; i< obj->parser.toknext - tokenIndex; i++ )
    {
        if( obj->parser.tokens[tokenIndex+i].parent <= parent )
        {
            range.count = i;//-1;
            break;
        }
    }

    return range;
}

SABool_t JsmnInterface_parse(struct JsmnInterface_t* obj, const char* js, size_t js_len)
{
    obj->js = (char*)SAMEM_MALLOC(sizeof(char) * js_len + 1);
    memcpy(obj->js, js, js_len);
    obj->js[js_len] = '\0';

    jsmn_init(&obj->parser);

    // struct JsmnInterface_token_t token;
    int parse_result = jsmn_parse(&obj->parser, obj->js, js_len);
    if(parse_result < 0)
    {
        if( parse_result == JSMN_ERROR_INVAL || parse_result == JSMN_ERROR_PART )
        {
            SALOG_ERROR("object_parser", "Inval json format");
        }
    
        SALOG_ERROR("object_parser", "Json parse failed");
        return SABOOL_FALSE;
    }

    return SABOOL_TRUE;
}


struct JsmnInterface_token_t JsmnInterface_getRoot(struct JsmnInterface_t* obj)
{
    struct JsmnInterface_token_t token;
    token.index_start = 0;
    token.count = obj->parser.toknext;

    return token;
}


void JsmnInterface_showResult(struct JsmnInterface_t* obj, struct JsmnInterface_token_t* token)
{
    for(size_t i = 0; i < token->count; i++)
    {
        SALOG_INFO("object_parser", "(index:%d) %d, %d", i, obj->parser.tokens[i + token->index_start].type, obj->parser.tokens[i + token->index_start].parent);
    }
}


void JsmnInterface_destructor(struct JsmnInterface_t* obj)
{
    jsmn_destructor(&obj->parser);
    SAMEM_FREE(obj->js);
}


jsmntype_t JsmnInterface_jsonType(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token)
{
    return obj->parser.tokens[token->index_start].type;
}


SABool_t JsmnInterface_getDictionary(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, const char* key, struct JsmnInterface_token_t* result)
{
    result->index_start = 0;
    result->count = 0;
    size_t tokenLen = strlen( key );

    SABool_t is_found = SABOOL_FALSE;

    if(obj->parser.tokens[token->index_start].type == JSMN_OBJECT)
    {
        for(size_t i = 0; i < token->count; i++)
        {
            if(obj->parser.tokens[i + token->index_start].parent == (int)token->index_start)
            {
                size_t keyLength = (size_t)(obj->parser.tokens[i + token->index_start].end - obj->parser.tokens[i + token->index_start].start);
                if(tokenLen == keyLength && (strncmp( &obj->js[obj->parser.tokens[i + token->index_start].start], key, keyLength )== 0))
                {
                    *result = JsmnInterface_searchTokCurRange(obj, i + token->index_start + 1);
                    is_found = SABOOL_TRUE;
                    break;
                }
            }
        }
        return is_found;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t JsmnInterface_dictionaryCount(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t* result)
{
    *result = 0;
    if(obj->parser.tokens[token->index_start].type == JSMN_OBJECT)
    {
        for(size_t i = 0; i < token->count; i++)
        {
            if(obj->parser.tokens[i + token->index_start].parent == (int)token->index_start)
            {
                (*result)++;
            }
        }
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t JsmnInterface_getDictionaryAtIndex(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t dic_index, char* key, struct JsmnInterface_token_t* result)
{
    result->count = 0;
    result->index_start = 0;

    size_t count = 0;
    SABool_t is_found = SABOOL_FALSE;

    if(obj->parser.tokens[token->index_start].type == JSMN_OBJECT)
    {
        for(size_t i = 0; i < token->count; i++)
        {
            if(obj->parser.tokens[i + token->index_start].parent == (int)token->index_start)
            {
                if(dic_index == count)
                {
                    size_t key_length = (size_t)(obj->parser.tokens[i + token->index_start].end - obj->parser.tokens[i + token->index_start].start);
                    strncpy(key, &obj->js[obj->parser.tokens[i + token->index_start].start], key_length);
                    key[key_length] = '\0';
                    *result = JsmnInterface_searchTokCurRange(obj, i + token->index_start + 1);
                    is_found = SABOOL_TRUE;
                    break;
                }
                count++;
            }
        }
        return is_found;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t JsmnInterface_arrayCount(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t* result)
{
    *result = 0;

    if(obj->parser.tokens[token->index_start].type == JSMN_ARRAY)
    {
        for(size_t i = 0; i < token->count; i++)
        {
            if(obj->parser.tokens[i + token->index_start].parent == (int)token->index_start)
            {
                (*result)++;
            }
        }
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t JsmnInterface_getArray(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t index, struct JsmnInterface_token_t* result)
{    
    result->count = 0;
    result->index_start = 0;

    size_t count = 0;
    SABool_t is_found = SABOOL_FALSE;

    if(obj->parser.tokens[token->index_start].type == JSMN_ARRAY)
    {
        for(size_t i = 0; i < token->count; i++)
        {
            if(obj->parser.tokens[i + token->index_start].parent == (int)token->index_start)
            {
                if(count == index)
                {
                    *result = JsmnInterface_searchTokCurRange( obj, i + token->index_start);
                    is_found = SABOOL_TRUE;
                    break;
                }
                count++;
            }
        }
        return is_found;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t JsmnInterface_parseAsInt(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, int* result)
{
    if((token->count == 1) && (obj->parser.tokens[token->index_start].type == JSMN_INT))
    {
        int start = obj->parser.tokens[token->index_start].start;
        int end = obj->parser.tokens[token->index_start].end;

        const char* buffer = &obj->js[start];
        char tmp_char = obj->js[end];
        obj->js[end] = 0x00;

        *result = atoi(buffer);

        obj->js[end] = tmp_char;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}

SABool_t JsmnInterface_parseAsLong(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, long* result)
{
    if((token->count == 1) && (obj->parser.tokens[token->index_start].type == JSMN_INT))
    {
        int start = obj->parser.tokens[token->index_start].start;
        int end = obj->parser.tokens[token->index_start].end;

        const char* buffer = &obj->js[start];
        char tmp_char = obj->js[end];
        obj->js[end] = 0x00;

        *result = atol(buffer);

        obj->js[end] = tmp_char;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t JsmnInterface_parseAsDouble(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, double* result)
{
    if((token->count == 1) && (obj->parser.tokens[token->index_start].type == JSMN_DOUBLE))
    {
        int start = obj->parser.tokens[token->index_start].start;
        int end = obj->parser.tokens[token->index_start].end;

        const char* buffer = &obj->js[start];
        char tmp_char = obj->js[end];
        obj->js[end] = 0x00;

        *result = atof(buffer);

        obj->js[end] = tmp_char;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t JsmnInterface_parseAsBoolean(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, SABool_t* result)
{
    SABool_t is_success = SABOOL_TRUE;
    if(token->count == 1)
    {
        if(obj->parser.tokens[token->index_start].type == JSMN_TRUE)
        {
            *result = SABOOL_TRUE;
        }else if(obj->parser.tokens[token->index_start].type == JSMN_FALSE){
            *result = SABOOL_FALSE;
        }else{
            is_success = SABOOL_FALSE;
        }
    }else{
        is_success = SABOOL_FALSE;
    }
    return is_success;
}


SABool_t JsmnInterface_stringLen(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t* result)
{
    if((token->count == 1) && (obj->parser.tokens[token->index_start].type == JSMN_STRING))
    {
        *result = obj->parser.tokens[token->index_start].end - obj->parser.tokens[token->index_start].start;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t JsmnInterface_parseAsString(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t copy_len, char* buff_out, size_t* len_out)
{
    if((token->count == 1) && (obj->parser.tokens[token->index_start].type == JSMN_STRING))
    {
        if( (obj->parser.tokens[token->index_start].end - obj->parser.tokens[token->index_start].start) < ((int)copy_len - 1) )
        {
            copy_len = obj->parser.tokens[token->index_start].end - obj->parser.tokens[token->index_start].start;
        }

        copy_len -= 1;

        memcpy(buff_out, &obj->js[obj->parser.tokens[token->index_start].start], sizeof(char) * copy_len);
        buff_out[copy_len] = 0x00;
        *len_out = copy_len;
        return SABOOL_TRUE;
    }else{
        *len_out = 0;
        return SABOOL_FALSE;
    }
}

SABool_t JsmnInterface_getStringRaw(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, char** result)
{    
    if ( (token->count == 1) && (obj->parser.tokens[token->index_start].type == JSMN_STRING) )
    {
        *result = &obj->js[obj->parser.tokens[token->index_start].start];
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}
