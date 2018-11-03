#include <ogg/ogg.h>

#ifndef __OPUSTAGS_H
#define __OPUSTAGS_H

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
} opus_tags;

int parse_tags(char *data, long len, opus_tags *tags);
int render_tags(opus_tags *tags, ogg_packet *op);
int match_field(const char *comment, uint32_t len, const char *field);
void delete_tags(opus_tags *tags, const char *field);
int add_tags(opus_tags *tags, const char **tags_to_add, uint32_t count);
void print_tags(opus_tags *tags);
void free_tags(opus_tags *tags);
int write_page(ogg_page *og, FILE *stream);

// opus-tools 0.1.10: picture.h

typedef enum{
  PIC_FORMAT_JPEG,
  PIC_FORMAT_PNG,
  PIC_FORMAT_GIF
}picture_format;

#define IMAX(a,b) ((a) > (b) ? (a) : (b))
#define BASE64_LENGTH(len) (((len)+2)/3*4)

/*Utility function for base64 encoding METADATA_BLOCK_PICTURE tags.
  Stores BASE64_LENGTH(len)+1 bytes in dst (including a terminating NUL).*/
void base64_encode(char *dst, const char *src, int len);

int oi_strncasecmp(const char *a, const char *b, int n);

int is_jpeg(const unsigned char *buf, size_t length);
int is_png(const unsigned char *buf, size_t length);
int is_gif(const unsigned char *buf, size_t length);

void extract_png_params(const unsigned char *data, size_t data_length,
                        ogg_uint32_t *width, ogg_uint32_t *height,
                        ogg_uint32_t *depth, ogg_uint32_t *colors,
                        int *has_palette);
void extract_gif_params(const unsigned char *data, size_t data_length,
                        ogg_uint32_t *width, ogg_uint32_t *height,
                        ogg_uint32_t *depth, ogg_uint32_t *colors,
                        int *has_palette);
void extract_jpeg_params(const unsigned char *data, size_t data_length,
                         ogg_uint32_t *width, ogg_uint32_t *height,
                         ogg_uint32_t *depth, ogg_uint32_t *colors,
                         int *has_palette);

char *parse_picture_specification(const char *spec,
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

#endif /* __OPUSTAGS_H */
