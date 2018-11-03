#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "opustags.h"

const char *version = PNAME " version " PVERSION "\nhttps://github.com/zvezdochiot/opustags [fork](https://github.com/fmang/opustags)\n";

const char *usage =
    "Usage: " PNAME " --help\n"
    "       " PNAME " [OPTIONS] FILE\n"
    "       " PNAME " OPTIONS FILE -o FILE\n";

const char *help =
    "Options:\n"
    "  -h, --help              print this help\n"
    "  -o, --output FILE       write the modified tags to a file\n"
    "  -i, --in-place [SUFFIX] use a temporary file then replace the original file\n"
    "  -y, --overwrite         overwrite the output file if it already exists\n"
    "  -v, --verbose           verbose (debug) mode (for corrupt file and development)\n"
    "  -d, --delete FIELD      delete all the fields of a specified type\n"
    "  -a, --add FIELD=VALUE   add a field\n"
    "  -s, --set FIELD=VALUE   delete then add a field\n"
    "  -p, --picture FILE      add cover\n"
    "  -D, --delete-all        delete all the fields!\n"
    "  -S, --set-all           read the fields from stdin\n";

struct option options[] = {
    {"help", no_argument, 0, 'h'},
    {"output", required_argument, 0, 'o'},
    {"in-place", optional_argument, 0, 'i'},
    {"overwrite", no_argument, 0, 'y'},
    {"verbose", no_argument, 0, 'v'},
    {"delete", required_argument, 0, 'd'},
    {"add", required_argument, 0, 'a'},
    {"set", required_argument, 0, 's'},
    {"picture", required_argument, 0, 'p'},
    {"delete-all", no_argument, 0, 'D'},
    {"set-all", no_argument, 0, 'S'},
    {NULL, 0, 0, 0}
};

int main(int argc, char **argv)
{
    if(argc == 1)
    {
        fputs(version, stdout);
        fputs(usage, stdout);
        return EXIT_SUCCESS;
    }
    char *path_in, *path_out = NULL, *inplace = NULL, *path_picture = NULL, *picture_data = NULL;
    const char* to_add[argc];
    const char* to_delete[argc];
    const char* to_picture[1];
    const char *error_message;
    int count_add = 0, count_delete = 0;
    int delete_all = 0;
    int set_all = 0;
    int overwrite = 0;
    int verbose = 0;
    int print_help = 0;
    int c;
    while((c = getopt_long(argc, argv, "ho:i::yvd:a:s:p:DS", options, NULL)) != -1)
    {
        switch(c){
            case 'h':
                print_help = 1;
                break;
            case 'o':
                path_out = optarg;
                break;
            case 'i':
                inplace = optarg == NULL ? ".otmp" : optarg;
                break;
            case 'y':
                overwrite = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'd':
                if(strchr(optarg, '=') != NULL)
                {
                    fprintf(stderr, "invalid field: '%s'\n", optarg);
                    return EXIT_FAILURE;
                }
                to_delete[count_delete++] = optarg;
                break;
            case 'a':
            case 's':
                if(strchr(optarg, '=') == NULL){
                    fprintf(stderr, "invalid comment: '%s'\n", optarg);
                    return EXIT_FAILURE;
                }
                to_add[count_add++] = optarg;
                if(c == 's')
                    to_delete[count_delete++] = optarg;
                break;
            case 'p':
                path_picture = optarg;
                break;
            case 'S':
                set_all = 1;
            case 'D':
                delete_all = 1;
                break;
            default:
                return EXIT_FAILURE;
        }
    }
    if(print_help)
    {
        puts(version);
        puts(usage);
        puts(help);
        puts("See the man page for extensive documentation.");
        return EXIT_SUCCESS;
    }
    if(optind != argc - 1)
    {
        fputs("invalid arguments\n", stderr);
        return EXIT_FAILURE;
    }
    if(inplace && path_out)
    {
        fputs("cannot combine --in-place and --output\n", stderr);
        return EXIT_FAILURE;
    }
    path_in = argv[optind];
    if(path_out != NULL && strcmp(path_in, "-") != 0)
    {
        char canon_in[PATH_MAX+1], canon_out[PATH_MAX+1];
        if(realpath(path_in, canon_in) && realpath(path_out, canon_out))
        {
            if(strcmp(canon_in, canon_out) == 0)
            {
                fputs("error: the input and output files are the same\n", stderr);
                return EXIT_FAILURE;
            }
        }
    }
    FILE *in;
    if(strcmp(path_in, "-") == 0)
    {
        if(set_all)
        {
            fputs("can't open stdin for input when -S is specified\n", stderr);
            return EXIT_FAILURE;
        }
        if(inplace)
        {
            fputs("cannot modify stdin 'in-place'\n", stderr);
            return EXIT_FAILURE;
        }
        in = stdin;
    }
    else
        in = fopen(path_in, "r");
    if(!in)
    {
        perror("fopen");
        return EXIT_FAILURE;
    }
    FILE *out = NULL;
    if(inplace != NULL)
    {
        path_out = malloc(strlen(path_in) + strlen(inplace) + 1);
        if(path_out == NULL)
        {
            fputs("failure to allocate memory\n", stderr);
            fclose(in);
            return EXIT_FAILURE;
        }
        strcpy(path_out, path_in);
        strcat(path_out, inplace);
    }
    if(path_out != NULL)
    {
        if(strcmp(path_out, "-") == 0)
            out = stdout;
        else{
            if(!overwrite && !inplace)
            {
                if(access(path_out, F_OK) == 0)
                {
                    fprintf(stderr, "'%s' already exists (use -y to overwrite)\n", path_out);
                    fclose(in);
                    return EXIT_FAILURE;
                }
            }
            out = fopen(path_out, "w");
            if(!out)
            {
                perror("fopen");
                fclose(in);
                if(inplace)
                    free(path_out);
                return EXIT_FAILURE;
            }
        }
    }
    if(path_picture != NULL)
    {
        int seen_file_icons=0;
        picture_data = parse_picture_specification(path_picture, &error_message, &seen_file_icons);
        if(picture_data == NULL)
        {
            fprintf(stderr,"Not read picture: %s\n", error_message);
        }
    }
    ogg_sync_state oy;
    ogg_stream_state os, enc;
    ogg_page og;
    ogg_packet op;
    opus_tags tags;
    ogg_sync_init(&oy);
    int ogg_buffer_size = 65536;
    char *buf;
    size_t len;
    char *error = NULL;
    int packet_count = -1;
    while(error == NULL)
    {
        // Read until we complete a page.
        if(ogg_sync_pageout(&oy, &og) != 1)
        {
            if(feof(in))
                break;
            buf = ogg_sync_buffer(&oy, ogg_buffer_size);
            if(buf == NULL)
            {
                error = "ogg_sync_buffer: out of memory";
                break;
            }
            len = fread(buf, 1, ogg_buffer_size, in);
            if(ferror(in))
                error = strerror(errno);
            ogg_sync_wrote(&oy, len);
            if(ogg_sync_check(&oy) != 0)
                error = "ogg_sync_check: internal error";
            continue;
        }
        // We got a page.
        // Short-circuit when the relevant packets have been read.
        if(packet_count >= 2 && out)
        {
            if(write_page(&og, out) == -1)
            {
                error = "write_page: fwrite error";
                if (!verbose)
                {
                    break;
                }
            }
            continue;
        }
        // Initialize the streams from the first page.
        if(packet_count == -1){
            if(ogg_stream_init(&os, ogg_page_serialno(&og)) == -1)
            {
                error = "ogg_stream_init: couldn't create a decoder";
                if (!verbose)
                {
                    break;
                }
            }
            if(out){
                if(ogg_stream_init(&enc, ogg_page_serialno(&og)) == -1)
                {
                    error = "ogg_stream_init: couldn't create an encoder";
                    if (!verbose)
                    {
                        break;
                    }
                }
            }
            packet_count = 0;
        }
        if(ogg_stream_pagein(&os, &og) == -1)
        {
            error = "ogg_stream_pagein: invalid page";
            {
                break;
            }
        }
        // Read all the packets.
        while(ogg_stream_packetout(&os, &op) == 1)
        {
            packet_count++;
            if(packet_count == 1)
            { // Identification header
                if(strncmp((char*) op.packet, "OpusHead", 8) != 0)
                {
                    error = "opustags: invalid identification header";
                    if (!verbose)
                    {
                        break;
                    }
                }
            }
            else if(packet_count == 2){ // Comment header
                if(parse_tags((char*) op.packet, op.bytes, &tags) == -1)
                {
                    error = "opustags: invalid comment header";
                    if (!verbose)
                    {
                        break;
                    }
                }
                if(delete_all)
                    tags.count = 0;
                else
                {
                    int i;
                    for(i=0; i<count_delete; i++)
                        delete_tags(&tags, to_delete[i]);
                }
                char *raw_tags = NULL;
                if(set_all){
                    size_t block_size = 4096, raw_buffer_size = 0, raw_len = 0;
                    int raw_count_max = 0;
                    raw_buffer_size = block_size;
                    raw_tags = malloc(raw_buffer_size);
                    if(raw_tags == NULL)
                    {
                        error = "malloc: not enough memory for buffering stdin";
                        free(raw_tags);
                        break;
                    }
                    else{
                        char ch;
                        ch = getc(stdin);
                        while (ch != EOF)
                        {
                            if (raw_len == raw_buffer_size)
                            {
                                raw_buffer_size += block_size;
                                raw_tags = (char*)realloc(raw_tags, raw_buffer_size * sizeof(char));
                                if(raw_tags == NULL)
                                {
                                    error = "realloc: not enough memory for buffering stdin";
                                    free(raw_tags);
                                    break;
                                }
                            }
                            if (ch == EOF) ch = '\0';
                            if (ch == '\n') raw_count_max++;
                            raw_tags[raw_len] = ch;
                            raw_len++;
                            ch = getc(stdin);
                        }
                        raw_count_max++;
                        char *raw_comment[raw_count_max];
                        uint32_t raw_count = 0;
                        size_t field_len = 0;
                        int caught_eq = 0;
                        int caught_cmnt = 0;
                        size_t i = 0;
                        char *cursor = raw_tags;
                        for(i=0; i <= raw_len && raw_count < raw_count_max; i++)
                        {
                            if(raw_tags[i] == '\n' || raw_tags[i] == '\0')
                            {
                                if(!caught_cmnt && field_len > 0)
                                {
                                    if(caught_eq)
                                        raw_comment[raw_count++] = cursor;
                                    else
                                        fputs("warning: skipping malformed tag\n", stderr);
                                }
                                cursor = raw_tags + i + 1;
                                field_len = 0;
                                caught_cmnt = 0;
                                caught_eq = 0;
                                raw_tags[i] = '\0';
                                continue;
                            }
                            if(raw_tags[i] == ' ' && field_len == 0)
                            {
                                cursor = raw_tags + i + 1;
                            } else {
                                if(raw_tags[i] == '#' && field_len == 0)
                                    caught_cmnt = 1;
                                if(raw_tags[i] == '=')
                                    caught_eq = 1;
                                field_len++;
                           }
                        }
                        add_tags(&tags, (const char**) raw_comment, raw_count);
                    }
                }
                add_tags(&tags, to_add, count_add);
                if(picture_data != NULL)
                {
                    char *picture_tag = "METADATA_BLOCK_PICTURE";
                    char *picture_meta = NULL;
                    int picture_meta_len = strlen(picture_tag) + strlen(picture_data) + 1;
                    picture_meta = malloc(picture_meta_len);
                    if(picture_meta == NULL)
                    {
                        fprintf(stderr,"Bad picture size: %d\n", picture_meta_len);
                        free(picture_meta);
                    } else {
                        delete_tags(&tags, picture_tag);
                        strcpy(picture_meta, picture_tag);
                        strcat(picture_meta, "=");
                        strcat(picture_meta, picture_data);
                        strcat(picture_meta, "\0");
                        to_picture[0] = picture_meta;
                        add_tags(&tags, to_picture, 1);
                    }
                }
                if(out)
                {
                    ogg_packet packet;
                    render_tags(&tags, &packet);
                    if(ogg_stream_packetin(&enc, &packet) == -1)
                        error = "ogg_stream_packetin: internal error";
                    free(packet.packet);
                }
                else
                    print_tags(&tags);
                free_tags(&tags);
                if(raw_tags)
                    free(raw_tags);
                if(error || !out)
                    break;
                else
                    continue;
            }
            if(out)
            {
                if(ogg_stream_packetin(&enc, &op) == -1)
                {
                    error = "ogg_stream_packetin: internal error";
                    break;
                }
            }
        }
        if(error != NULL)
            break;
        if(ogg_stream_check(&os) != 0)
            error = "ogg_stream_check: internal error (decoder)";
        // Write the page.
        if(out){
            while(ogg_stream_flush(&enc, &og))
            {
                if(write_page(&og, out) == -1)
                    error = "write_page: fwrite error";
                else if(ogg_stream_check(&enc) != 0)
                    error = "ogg_stream_check: internal error (encoder)";
            }
        }
        else if(packet_count >= 2) // Read-only mode
            break;
    }
    if(packet_count >= 0)
    {
        ogg_stream_clear(&os);
        if(out)
            ogg_stream_clear(&enc);
    }
    ogg_sync_clear(&oy);
    fclose(in);
    if(out)
        fclose(out);
    if(!error && packet_count < 2)
        error = "opustags: invalid file";
    if(error)
    {
        fprintf(stderr, "%s\n", error);
        if(path_out != NULL && out != stdout)
            remove(path_out);
        if(inplace)
            free(path_out);
        return EXIT_FAILURE;
    }
    else if(inplace)
    {
        if(rename(path_out, path_in) == -1)
        {
            perror("rename");
            free(path_out);
            return EXIT_FAILURE;
        }
        free(path_out);
    }
    return EXIT_SUCCESS;
}
