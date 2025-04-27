#ifndef DECODE_H
#define DECODE_H
 // Contains user defined types
#include "types.h"

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;

    /* Secret File Info */
    char stego_image_fname[20];
    FILE *fptr_stego_image;
    char extn_output_file[MAX_FILE_SUFFIX];
    char output_data[MAX_SECRET_BUF_SIZE];
    long size_output_file;

} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status dopen_files(DecodeInfo *decInfo);
Status skip_bmp_header(FILE *fptr_src_image);

/* Store Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Encode secret file size */
Status decode_secret_file_extn( DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);
/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Encode function, which does the real encoding */
Status decode_data_to_image(char *data, int size, FILE *fptr_src_image,DecodeInfo *decInfo);

/* Encode a byte into LSB of image data array */
Status decode_lsb_to_byte(char *buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status decode_size_to_lsb(char *buffer, long int *size);

#endif
