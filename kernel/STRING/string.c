#include "string.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"
#include "../STD/defs.h"
#include "../STD/bool.h"

str_error_t str_last_err = STR_ERR_NONE;

void str_set_error(str_error_t err)
{
    str_last_err = err;
}

str_error_t str_get_error(void)
{
    return str_last_err;
}

// optional: simple strlen
int strlen(const char *s)
{
    if (!s)
    {
        str_set_error(STR_ERR_INVALID_ARG);
        return 0;
    }

    str_set_error(STR_ERR_NONE);
    int len = 0;
    while (*s++) len++;
    return len;
}

int strcmp(const char *a, const char *b)
{
    if (!a || !b)
    {
        str_set_error(STR_ERR_INVALID_ARG);
        return 0;
    }

    str_set_error(STR_ERR_NONE);
    while (*a && (*a == *b)) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}


char *strncpy(char *dest, const char *src, int n)
{
    if (!dest || !src || n < 0)
    {
        str_set_error(STR_ERR_INVALID_ARG);
        return NULL;
    }

    int i;
    for (i = 0; i < n && src[i]; i++)
        dest[i] = src[i];

    for (; i < n; i++)
        dest[i] = 0;

    str_set_error(STR_ERR_NONE);
    return dest;
}


char *strncat(char *dest, const char *src, int n)
{
    if (!dest || !src || n < 0)
    {
        str_set_error(STR_ERR_INVALID_ARG);
        return NULL;
    }

    int i = 0;
    while (dest[i])
        i++;

    int j = 0;
    for (; j < n && src[j]; j++)
        dest[i + j] = src[j];

    dest[i + j] = 0;

    str_set_error(STR_ERR_NONE);
    return dest;
}



/**
 * Converts a UTF-16 (ASCII-only) buffer to ASCII.
 * Non-ASCII characters are replaced with '?'.
 *
 * @param src   Pointer to UTF-16 source
 * @param len   Number of UTF-16 code units in src
 * @param dest  Pointer to ASCII destination buffer (must be at least len + 1 bytes)
 * @return      Number of ASCII bytes written (excluding null terminator)
 */
uint32_t utf16_to_ascii(uint16_t *src, uint32_t len, char *dest)
{
    if (!src || !dest)
    {
        str_set_error(STR_ERR_INVALID_ARG);
        return 0;
    }

    for (uint32_t i = 0; i < len; i++)
    {
        uint16_t ch = src[i];
        dest[i] = (ch <= 0x7F) ? (char)ch : '?';
    }
    dest[len] = '\0';

    str_set_error(STR_ERR_NONE);
    return len;
}


uint16_t strsplit(const char *str, const char delimiter, char ***dest)
{
    if (!str || !dest)
    {
        str_set_error(STR_ERR_INVALID_ARG);
        return 0;
    }

    str_set_error(STR_ERR_NONE);
    *dest = (char**)NULL;
    uint16_t count = 0;
    char *buffer = (char *)NULL;
    uint16_t current_count = 0;
    uint32_t i = 0;
    void *tmp;
    while(true)
    {
        if(str[i] == '\0')
        {
            if(buffer && current_count > 0)
            {

                tmp = realloc(buffer, current_count + 1);
                if(!tmp)
                {
                    str_set_error(STR_ERR_ALLOC_FAIL);
                    goto fail;
                }
                buffer = (char*)tmp;
                tmp = NULL;

                buffer[current_count] = '\0';

                tmp = realloc((*dest), (count + 1) * sizeof(char*));
                if(!tmp)
                {
                    str_set_error(STR_ERR_ALLOC_FAIL);
                    goto fail;
                }
                (*dest) = (char**)tmp;
                tmp = NULL;

                (*dest)[count] = buffer;
                
                buffer = (char *)NULL;
                
                count++;
            }
            str_set_error(STR_ERR_NONE);
            return count;

        }
        if(str[i] == delimiter)
        {
                if(buffer && current_count > 0)
                {
                    tmp = realloc(buffer, current_count + 1);
                    if(!tmp)
                    {
                        str_set_error(STR_ERR_ALLOC_FAIL);
                        goto fail;
                    }
                    buffer = (char*)tmp;
                    tmp = NULL;

                    buffer[current_count] = '\0';
    
                    tmp = realloc((*dest), (count + 1) * sizeof(char*));
                    if(!tmp)
                    {
                        str_set_error(STR_ERR_ALLOC_FAIL);
                        goto fail;
                    }
                    (*dest) = (char**)tmp;
                    tmp = NULL;

                    (*dest)[count] = buffer;
                    count++;
                }
                
                buffer = (char *)NULL;
                current_count = 0;
                i++;            
        }
        else
        {
            tmp = realloc(buffer, current_count + 1);
            if(!tmp)
            {
                str_set_error(STR_ERR_ALLOC_FAIL);
                goto fail;
            }
            buffer = (char*)tmp;
            tmp = NULL;

            buffer[current_count] = str[i];
            current_count++;
            i++;

        }
    }

    fail:
    // Free partially built buffer
    if (buffer)
        free(buffer);

    // Free already stored tokens
    if (*dest)
    {
        for (uint16_t j = 0; j < count; j++)
            free((*dest)[j]);

        free(*dest);
        *dest = (char**)NULL;
    }
    return 0;
}





#include "../STD/int.h"
#include "../STD/bool.h"

// reverse string in place
static void reverse(char *str, uint32_t len) {
    uint32_t i = 0, j = len - 1;
    while (i < j) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
        i++; j--;
    }
}

// convert integer to string (decimal or hex)
static uint32_t itoa(uint32_t val, char *buf, uint8_t base, bool uppercase) {
    char digits_lo[] = "0123456789abcdef";
    char digits_up[] = "0123456789ABCDEF";
    char *digits = uppercase ? digits_up : digits_lo;

    uint32_t i = 0;
    if (val == 0) {
        buf[i++] = '0';
        buf[i] = 0;
        return i;
    }

    while (val) {
        buf[i++] = digits[val % base];
        val /= base;
    }
    buf[i] = 0;
    reverse(buf, i);
    return i;
}

// minimal snprintf with only %d, %u, %x, %X, %s, %%
uint32_t snprintf(char *buf, uint32_t size, const char *fmt, void **args) {
    uint32_t pos = 0;
    uint32_t arg_idx = 0;

    for (uint32_t i = 0; fmt[i] && pos < size - 1; i++) {
        if (fmt[i] != '%') {
            buf[pos++] = fmt[i];
            continue;
        }
        i++;
        if (!fmt[i]) break;

        if (fmt[i] == '%') {
            buf[pos++] = '%';
        } else if (fmt[i] == 'd') {
            int32_t val = (int32_t)(uintptr_t)args[arg_idx++];
            if (val < 0) {
                buf[pos++] = '-';
                val = -val;
            }
            char tmp[32];
            uint32_t len = itoa((uint32_t)val, tmp, 10, false);
            for (uint32_t j = 0; j < len && pos < size - 1; j++)
                buf[pos++] = tmp[j];
        } else if (fmt[i] == 'u') {
            uint32_t val = (uint32_t)(uintptr_t)args[arg_idx++];
            char tmp[32];
            uint32_t len = itoa(val, tmp, 10, false);
            for (uint32_t j = 0; j < len && pos < size - 1; j++)
                buf[pos++] = tmp[j];
        } else if (fmt[i] == 'x' || fmt[i] == 'X') {
            uint32_t val = (uint32_t)(uintptr_t)args[arg_idx++];
            char tmp[32];
            uint32_t len = itoa(val, tmp, 16, fmt[i] == 'X');
            for (uint32_t j = 0; j < len && pos < size - 1; j++)
                buf[pos++] = tmp[j];
        } else if (fmt[i] == 's') {
            char *s = (char*)args[arg_idx++];
            for (uint32_t j = 0; s[j] && pos < size - 1; j++)
                buf[pos++] = s[j];
        }
    }

    buf[pos] = 0;
    return pos;
}
