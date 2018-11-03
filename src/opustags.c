#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opustags.h"

int parse_tags(char *data, long len, opus_tags *tags)
{
    long pos;
    if(len < 8+4+4)
        return -1;
    if(strncmp(data, "OpusTags", 8) != 0)
        return -1;
    // Vendor
    pos = 8;
    tags->vendor_length = le32toh(*((uint32_t*) (data + pos)));
    tags->vendor_string = data + pos + 4;
    pos += 4 + tags->vendor_length;
    if(pos + 4 > len)
        return -1;
    // Count
    tags->count = le32toh(*((uint32_t*) (data + pos)));
    if(tags->count == 0)
        return 0;
    tags->lengths = calloc(tags->count, sizeof(uint32_t));
    if(tags->lengths == NULL)
        return -1;
    tags->comment = calloc(tags->count, sizeof(char*));
    if(tags->comment == NULL)
    {
        free(tags->lengths);
        return -1;
    }
    pos += 4;
    // Comment
    uint32_t i;
    for(i=0; i<tags->count; i++)
    {
        tags->lengths[i] = le32toh(*((uint32_t*) (data + pos)));
        tags->comment[i] = data + pos + 4;
        pos += 4 + tags->lengths[i];
        if(pos > len)
            return -1;
    }

    if(pos < len)
        fprintf(stderr, "warning: %ld unused bytes at the end of the OpusTags packet\n", len - pos);

    return 0;
}

int render_tags(opus_tags *tags, ogg_packet *op)
{
    // Note: op->packet must be manually freed.
    op->b_o_s = 0;
    op->e_o_s = 0;
    op->granulepos = 0;
    op->packetno = 1;
    long len = 8 + 4 + tags->vendor_length + 4;
    uint32_t i;
    for(i = 0; i < tags->count; i++)
        len += 4 + tags->lengths[i];
    op->bytes = len;
    char *data = malloc(len);
    if(!data)
        return -1;
    op->packet = (unsigned char*) data;
    uint32_t n;
    memcpy(data, "OpusTags", 8);
    n = htole32(tags->vendor_length);
    memcpy(data+8, &n, 4);
    memcpy(data+12, tags->vendor_string, tags->vendor_length);
    data += 12 + tags->vendor_length;
    n = htole32(tags->count);
    memcpy(data, &n, 4);
    data += 4;
    for(i = 0; i < tags->count; i++)
    {
        n = htole32(tags->lengths[i]);
        memcpy(data, &n, 4);
        memcpy(data+4, tags->comment[i], tags->lengths[i]);
        data += 4 + tags->lengths[i];
    }
    return 0;
}

int match_field(const char *comment, uint32_t len, const char *field)
{
    size_t field_len;
    for(field_len = 0; field[field_len] != '\0' && field[field_len] != '='; field_len++);
    if(len <= field_len)
        return 0;
    if(comment[field_len] != '=')
        return 0;
    if(strncmp(comment, field, field_len) != 0)
        return 0;
    return 1;

}

void delete_tags(opus_tags *tags, const char *field)
{
    uint32_t i;
    for(i = 0; i < tags->count; i++)
    {
        if(match_field(tags->comment[i], tags->lengths[i], field))
        {
            tags->count--;
            tags->lengths[i] = tags->lengths[tags->count];
            tags->comment[i] = tags->comment[tags->count];
            // No need to resize the arrays.
        }
    }
}

int add_tags(opus_tags *tags, const char **tags_to_add, uint32_t count)
{
    if(count == 0)
        return 0;
    uint32_t *lengths = realloc(tags->lengths, (tags->count + count) * sizeof(uint32_t));
    const char **comment = realloc(tags->comment, (tags->count + count) * sizeof(char*));
    if(lengths == NULL || comment == NULL)
        return -1;
    tags->lengths = lengths;
    tags->comment = comment;
    uint32_t i;
    for(i = 0; i < count; i++)
    {
        tags->lengths[tags->count + i] = strlen(tags_to_add[i]);
        tags->comment[tags->count + i] = tags_to_add[i];
    }
    tags->count += count;
    return 0;
}

void print_tags(opus_tags *tags)
{
    if(tags->count == 0)
    {
        puts("#no tags");
        puts("# example: ARTIST=you song artist");
    }
    int i;
    for(i = 0; i < tags->count; i++)
    {
        fwrite(tags->comment[i], 1, tags->lengths[i], stdout);
        puts("");
    }
}

void free_tags(opus_tags *tags)
{
    if(tags->count > 0)
    {
        free(tags->lengths);
        free(tags->comment);
    }
}

int write_page(ogg_page *og, FILE *stream)
{
    if(fwrite(og->header, 1, og->header_len, stream) < og->header_len)
        return -1;
    if(fwrite(og->body, 1, og->body_len, stream) < og->body_len)
        return -1;
    return 0;
}

