#include "jsmn.h"

#include <stdlib.h>

#define JSMN_TOKEN_BLOCK_SIZE (64)

/**
 * Allocates a fresh unused token from the token pool.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser) {
    jsmntok_t *tok;
    if (parser->toknext >= parser->num_token) {
        parser->num_token += JSMN_TOKEN_BLOCK_SIZE;
        parser->tokens = (jsmntok_t*)realloc(parser->tokens, sizeof(jsmntok_t) * parser->num_token);
    }
    tok = &parser->tokens[parser->toknext++];
    tok->start = tok->end = -1;
    tok->size = 0;
    tok->parent = -1;
    return tok;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *token, const jsmntype_t type, const int start, const int end) {
    token->type = type;
    token->start = start;
    token->end = end;
    token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js, const size_t len) {
    jsmntok_t *token;
    int start;

    start = parser->pos;

    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
        switch (js[parser->pos]) {
#ifndef JSMN_STRICT
        /* In strict mode primitive must be followed by "," or "}" or "]" */
        case ':':
#endif
        case '\t':
        case '\r':
        case '\n':
        case ' ':
        case ',':
        case ']':
        case '}':
            goto found;
        default:
                                     /* to quiet a warning from gcc*/
            break;
        }
        if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
            parser->pos = start;
            return JSMN_ERROR_INVAL;
        }
    }
#ifdef JSMN_STRICT
    /* In strict mode primitive must be followed by a comma/object/array */
    parser->pos = start;
    return JSMN_ERROR_PART;
#endif

found:
    if (parser->tokens == NULL) {
        parser->pos--;
        return 0;
    }
    token = jsmn_alloc_token(parser);
    if (token == NULL) {
        parser->pos = start;
        return JSMN_ERROR_NOMEM;
    }

    jsmntype_t primitive_type = JSMN_INT;

    if(js[start] == 'f')
    {
        if(js[start + 1] == 'a' && js[start + 2] == 'l' && js[start + 3] == 's' && js[start + 4] == 'e')
        {
                primitive_type = JSMN_FALSE;
        }else{
                primitive_type = JSMN_UNDEFINED;
        }
    }
    else if(js[start] == 't')
    {
        if(js[start + 1] == 'r' && js[start + 2] == 'u' && js[start + 3] == 'e')
        {
                primitive_type = JSMN_TRUE;
        }else{
                primitive_type = JSMN_UNDEFINED;
        }
    }
    else
    {
        for(size_t i = start; i <= parser->pos; i++)
        {
            if(js[i] == '.')
            {
                    primitive_type = JSMN_DOUBLE;
                    break;
            }
        }
    }

    jsmn_fill_token(token, primitive_type, start, parser->pos);
    token->parent = parser->toksuper;

    parser->pos--;
    return 0;
}

/**
 * Fills next token with JSON string.
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js, const size_t len) {
    jsmntok_t *token;

    int start = parser->pos;

    parser->pos++;

    /* Skip starting quote */
    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
        char c = js[parser->pos];

        /* Quote: end of string */
        if (c == '\"') {
            if (parser->tokens == NULL) {
                return 0;
            }
            token = jsmn_alloc_token(parser);
            if (token == NULL) {
                parser->pos = start;
                return JSMN_ERROR_NOMEM;
            }

            jsmntype_t is_key = JSMN_STRING;
            token->parent = parser->toksuper;
            if(0 <= token->parent)
            {
                if(parser->tokens[token->parent].type == JSMN_OBJECT)
                {
                    is_key = JSMN_KEY;
                }   
            }
            jsmn_fill_token(token, is_key, start + 1, parser->pos);
            return 0;
        }

        /* Backslash: Quoted symbol expected */
        if (c == '\\' && parser->pos + 1 < len) {
            int i;
            parser->pos++;
            switch (js[parser->pos]) {
            /* Allowed escaped symbols */
            case '\"':
            case '/':
            case '\\':
            case 'b':
            case 'f':
            case 'r':
            case 'n':
            case 't':
                break;
            /* Allows escaped symbol \uXXXX */
            case 'u':
                parser->pos++;
                for (i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0';
                         i++) {
                    /* If it isn't a hex character we have an error */
                    if (!((js[parser->pos] >= 48 && js[parser->pos] <= 57) ||     /* 0-9 */
                                (js[parser->pos] >= 65 && js[parser->pos] <= 70) ||     /* A-F */
                                (js[parser->pos] >= 97 && js[parser->pos] <= 102))) { /* a-f */
                        parser->pos = start;
                        return JSMN_ERROR_INVAL;
                    }
                    parser->pos++;
                }
                parser->pos--;
                break;
            /* Unexpected symbol */
            default:
                parser->pos = start;
                return JSMN_ERROR_INVAL;
            }
        }
    }
    parser->pos = start;
    return JSMN_ERROR_PART;
}

/**
 * Parse JSON string and fill tokens.
 */
JSMN_API int jsmn_parse(jsmn_parser *parser, const char *js, const size_t len)
{
    int r;
    int i;
    jsmntok_t *token;
    int count = parser->toknext;

    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
        char c;
        jsmntype_t type;

        c = js[parser->pos];
        switch (c) {
        case '{':
        case '[':
            count++;
            token = jsmn_alloc_token(parser);
            if (token == NULL) {
                return JSMN_ERROR_NOMEM;
            }
            if (parser->toksuper != -1) {
                jsmntok_t *t = &parser->tokens[parser->toksuper];
#ifdef JSMN_STRICT
                /* In strict mode an object or array can't become a key */
                if (t->type == JSMN_OBJECT) {
                    return JSMN_ERROR_INVAL;
                }
#endif
                t->size++;
                token->parent = parser->toksuper;
            }
            token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
            token->start = parser->pos;
            parser->toksuper = parser->toknext - 1;
            break;
        case '}':
        case ']':
            if (parser->tokens == NULL) {
                break;
            }
            type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
            if (parser->toknext < 1) {
                return JSMN_ERROR_INVAL;
            }
            token = &parser->tokens[parser->toknext - 1];
            for (;;) {
                if (token->start != -1 && token->end == -1) {
                    if (token->type != type) {
                        return JSMN_ERROR_INVAL;
                    }
                    token->end = parser->pos + 1;
                    parser->toksuper = token->parent;
                    break;
                }
                if (token->parent == -1) {
                    if (token->type != type || parser->toksuper == -1) {
                        return JSMN_ERROR_INVAL;
                    }
                    break;
                }
                token = &parser->tokens[token->parent];
            }
            break;
        case '\"':
            r = jsmn_parse_string(parser, js, len);
            if (r < 0) {
                return r;
            }
            count++;
            if (parser->toksuper != -1 && parser->tokens != NULL) {
                parser->tokens[parser->toksuper].size++;
            }
            break;
        case '\t':
        case '\r':
        case '\n':
        case ' ':
            break;
        case ':':
            parser->toksuper = parser->toknext - 1;
            break;
        case ',':
            if (parser->tokens != NULL && parser->toksuper != -1 &&
                    parser->tokens[parser->toksuper].type != JSMN_ARRAY &&
                    parser->tokens[parser->toksuper].type != JSMN_OBJECT) {
                parser->toksuper = parser->tokens[parser->toksuper].parent;
            }
            break;
#ifdef JSMN_STRICT
        /* In strict mode primitives are: numbers and booleans */
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 't':
        case 'f':
        case 'n':
            /* And they must not be keys of the object */
            if (tokens != NULL && parser->toksuper != -1) {
                const jsmntok_t *t = &tokens[parser->toksuper];
                if (t->type == JSMN_OBJECT ||
                        (t->type == JSMN_STRING && t->size != 0)) {
                    return JSMN_ERROR_INVAL;
                }
            }
#else
        /* In non-strict mode every unquoted value is a primitive */
        default:
#endif
            r = jsmn_parse_primitive(parser, js, len);
            if (r < 0) {
                return r;
            }
            count++;
            if (parser->toksuper != -1 && parser->tokens != NULL) {
                parser->tokens[parser->toksuper].size++;
            }
            break;

#ifdef JSMN_STRICT
        /* Unexpected char in strict mode */
        default:
            return JSMN_ERROR_INVAL;
#endif
        }
    }

    if (parser->tokens != NULL) {
        for (i = parser->toknext - 1; i >= 0; i--) {
            /* Unmatched opened object or array */
            if (parser->tokens[i].start != -1 && parser->tokens[i].end == -1) {
                return JSMN_ERROR_PART;
            }
        }
    }

    return count;
}

/**
 * Creates a new parser based over a given buffer with an array of tokens
 * available.
 */
JSMN_API void jsmn_init(jsmn_parser *parser) {
    parser->pos = 0;
    parser->toknext = 0;
    parser->toksuper = -1;

    parser->num_token = JSMN_TOKEN_BLOCK_SIZE;
    parser->tokens = (jsmntok_t*)malloc(sizeof(jsmntok_t) * parser->num_token);
}

JSMN_API void jsmn_destructor(jsmn_parser *parser)
{
    free(parser->tokens);
}