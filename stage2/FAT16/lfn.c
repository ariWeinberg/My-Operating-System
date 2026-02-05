#include "fat16.h"
#include "../STRING/string.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"
#include "../UTILS/utils.h"

void lfn_buffer_clear(LFN_buffer *buf)
{
    buf->count = 0;
    fat16_ok();
}

bool parse_LFN_entry(const LFN_entry *lfn, LFN_buffer *buf)
{
    if (buf->count >= 20)
    {
        fat16_fail(LFN_BUFFER_FULL);
        return false;
    }

    parsed_LFN_entry *dst = &buf->entries[buf->count];

    dst->order    = lfn->order & 0x1F;
    dst->checksum = lfn->checksum;

    uint16_t tmp[13];
    uint32_t idx = 0;

    for (uint32_t i = 0; i < 5; i++) tmp[idx++] = lfn->value_first[i];
    for (uint32_t i = 0; i < 6; i++) tmp[idx++] = lfn->value_second[i];
    for (uint32_t i = 0; i < 2; i++) tmp[idx++] = lfn->value_third[i];

    utf16_to_ascii(tmp, 13, dst->name);

    buf->count++;
    fat16_ok();
    return true;
}

char *parse_LFN_buffer(LFN_buffer *buffer)
{
    if (buffer->count == 0)
    {
        fat16_fail(LFN_BUFFER_EMPTY);
        return NULL;
    }

    // Compute total length
    uint32_t total_len = buffer->count * 13;
    char *name = zalloc(total_len + 1);
    if (!name)
    {   fat16_fail(FAT16_MEMORY_ALLOCATION_FAIL);
        return NULL;
    }

    uint32_t pos = 0;

    // LFN entries are stored in reverse order
    for (int32_t i = buffer->count - 1; i >= 0; i--)
    {
        for (uint32_t j = 0; j < 13 && buffer->entries[i].name[j]; j++)
            name[pos++] = buffer->entries[i].name[j];
    }

    name[pos] = '\0';
    fat16_ok();
    return name;
}

char *normalize_SFN(const char sfn[11])
{
    // Max length: 8 + 1 + 3 + '\0' = 13
    char *out = zalloc(13);
    if (!out)
    {
        fat16_fail(FAT16_MEMORY_ALLOCATION_FAIL);
        return NULL;
    }

    uint32_t pos = 0;

    // ---- base name (bytes 0–7) ----
    for (uint32_t i = 0; i < 8; i++)
    {
        if (sfn[i] == ' ')
            break;
        out[pos++] = sfn[i];
    }

    // ---- extension (bytes 8–10) ----
    if (sfn[8] != ' ')
    {
        out[pos++] = '.';
        for (uint32_t i = 8; i < 11; i++)
        {
            if (sfn[i] == ' ')
                break;
            out[pos++] = sfn[i];
        }
    }

    out[pos] = '\0';
    fat16_ok();
    return out;
}
