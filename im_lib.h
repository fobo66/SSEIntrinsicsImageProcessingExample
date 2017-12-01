#ifndef IM_LIB_H
#define IM_LIB_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <xmmintrin.h>

typedef struct {
      char type1;               /* Two chars "BM" in little-endian order  */
      char type2;
    __int16_t size1;               /* File size                                 */
    __int16_t size2;               /* File size                              */
    __int16_t reserved1;
    __int16_t reserved2;
    __int16_t offset1;             /* Bytes from start of file to pixel data */
    __int16_t offset2;             /* Bytes from start of file to pixel data */
} BmpFileHeader;

typedef struct {
    __int32_t size;               /* Size of this header  (40 bytes)        */
    __int32_t  width;              /* Bitmap width in pixels                 */
    __int32_t  height;             /* Bitmap height in pixels                */
    __int16_t planes;             /* Must be 1                              */
    __int16_t bits;               /* Bits per pixel. (8 for 256-color bmps) */
    __int32_t compression;        /* 0 means no compression                 */
    __int32_t imagesize;          /* Number of bytes in pixel data section  */
    __int32_t  xresolution;        /* Not used by any of my functions        */
    __int32_t  yresolution;        /* Not used by any of my functions        */

    __int32_t ncolors;            /* 0 means use all colors                 */
    __int32_t importantcolors;    /* 0 means all are important              */
} BmpInfoHeader;

#define PI 3.1415926535897932384626433832795 // the value of PI

__uint8_t* imread(char const *file_name, BmpInfoHeader &im_info, BmpFileHeader &im_header, __uint8_t* color_map);
int imwrite(char const *file_name, __uint8_t* image, BmpInfoHeader &im_info, BmpFileHeader &im_header, __uint8_t* color_map);
float ** im2floatMtx(__uint8_t *data, __int32_t width, __int32_t height);
void floatMtx2im(__uint8_t *data, __int32_t width, __int32_t height, float **fMtx);
void findClosestCentroid(float **fMtx, __int32_t width, __int32_t height, float **cBook, int nColor, int* idx);
#endif
 
