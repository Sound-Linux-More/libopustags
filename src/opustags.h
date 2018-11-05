#include <ogg/ogg.h>

#ifndef __OPUSTAGS_H
#define __OPUSTAGS_H

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef __APPLE__
#include <libkern/OSByteOrder.h>
#define htole32(x) OSSwapHostToLittleInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)
#endif

typedef struct {
    uint32_t vendor_length;
    const char *vendor_string;
    uint32_t count;
    uint32_t *lengths;
    const char **comment;
} opustags_tags;

int opustags_tags_parse(char *data, long len, opustags_tags *tags);
int opustags_tags_render(opustags_tags *tags, ogg_packet *op);
int opustags_tags_match_field(const char *comment, uint32_t len, const char *field);
void opustags_tags_delete(opustags_tags *tags, const char *field);
int opustags_tags_add(opustags_tags *tags, const char **tags_to_add, uint32_t count);
void opustags_tags_print(opustags_tags *tags);
void opustags_tags_free(opustags_tags *tags);
int opustags_write_page(ogg_page *og, FILE *stream);

// opus-tools 0.1.10: picture.h

typedef enum{
  PIC_FORMAT_JPEG,
  PIC_FORMAT_PNG,
  PIC_FORMAT_GIF
}opustags_picture_format;

#define IMAX(a,b) ((a) > (b) ? (a) : (b))
#define BASE64_LENGTH(len) (((len)+2)/3*4)

/*Utility function for base64 encoding METADATA_BLOCK_PICTURE tags.
  Stores BASE64_LENGTH(len)+1 bytes in dst (including a terminating NUL).*/
void opustags_base64_encode(char *dst, const char *src, int len);

int opustags_oi_strncasecmp(const char *a, const char *b, int n);

int opustags_is_jpeg(const unsigned char *buf, size_t length);
int opustags_is_png(const unsigned char *buf, size_t length);
int opustags_is_gif(const unsigned char *buf, size_t length);

void opustags_params_extract_png(const unsigned char *data, size_t data_length,
                        ogg_uint32_t *width, ogg_uint32_t *height,
                        ogg_uint32_t *depth, ogg_uint32_t *colors,
                        int *has_palette);
void opustags_params_extract_gif(const unsigned char *data, size_t data_length,
                        ogg_uint32_t *width, ogg_uint32_t *height,
                        ogg_uint32_t *depth, ogg_uint32_t *colors,
                        int *has_palette);
void opustags_params_extract_jpeg(const unsigned char *data, size_t data_length,
                         ogg_uint32_t *width, ogg_uint32_t *height,
                         ogg_uint32_t *depth, ogg_uint32_t *colors,
                         int *has_palette);

char *opustags_picture_specification_parse(const char *spec,
                                  const char **error_message,
                                  int *seen_file_icons);

#define READ_U32_BE(buf) \
    (((buf)[0]<<24)|((buf)[1]<<16)|((buf)[2]<<8)|((buf)[3]&0xff))
#define WRITE_U32_BE(buf, val) \
  do{ \
    (buf)[0]=(unsigned char)((val)>>24); \
    (buf)[1]=(unsigned char)((val)>>16); \
    (buf)[2]=(unsigned char)((val)>>8); \
    (buf)[3]=(unsigned char)(val); \
  } \
  while(0);

#if defined(__cplusplus)
}
#endif

#endif /* __OPUSTAGS_H */
