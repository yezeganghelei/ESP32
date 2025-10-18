#ifndef _AVI_DEFINE_H_
#define _AVI_DEFINE_H_

#include "stdint.h"

/**
 * reference links: https://www.cnblogs.com/songhe364826110/p/7619949.html 
 * AVIFileFormat: https://web.archive.org/web/20170411001412/http://www.alexander-noe.com/video/documentation/avi.pdf
 * OpenDML AVI File Format Extensions: https://web.archive.org/web/20070112225112/http://www.the-labs.com/Video/odmlff2-avidef.pdf
 */

typedef struct {
    uint32_t FourCC; 
    uint32_t size;   //piece大小，Equal to the size of the subsequent data
    /* data uint8_t _data[size]; */
}AVI_CHUNK_HEAD;

typedef struct {
    uint32_t List;   //fixed to"LIST"，like果是 RIFF List Then it is "RIFF"
    uint32_t size;   //piece大小，Equal to the size of the subsequent data
    uint32_t FourCC;   
    /* Data uint8_t _data[size-4]; */
}AVI_LIST_HEAD;

typedef struct {
    uint32_t FourCC;            //Block ID, fixed to avih
    uint32_t size;              //Block size, equal to the size of struct avi_avih_chunk minus id and size
    uint32_t us_per_frame;      //Video frame interval time in microseconds
    uint32_t max_bytes_per_sec; //AVIThe maximum data rate of the file
    uint32_t padding;           //Just set it to 0
    uint32_t flags;             //AVI file global attributes, such as whether it contains index blocks, whether audio and video data are cross-stored, etc.
    uint32_t total_frames;      //total frames
    uint32_t init_frames;       //Specifies the initial frame number for interactive formats(Non-interactive format should be specified as0)
    uint32_t streams;           //The number of streams contained in the file，When only video stream is1
    uint32_t suggest_buff_size; //Specify the buffer size recommended for reading this file, usually the sum of the data required to store a frame of image and synchronized sound. If not specified, set to 0
    uint32_t width;             //Video main window width (unit: pixels)
    uint32_t height;            //Main video window height (unit: pixels)
    uint32_t reserved[4];       //Reserved valuedwScale,dwRate,dwStart,dwLength
}AVI_AVIH_CHUNK;

typedef struct {
    int16_t left;
    int16_t top;
    int16_t right;
    int16_t bottom;    
}AVI_RECT_FRAME;

typedef struct
{    
    uint32_t FourCC;            //pieceID，fixed tostrh
    uint32_t size;              //piece大小，wait于struct avi_strh_chunkremoveidandsizesize
    uint32_t fourcc_type;   //stream type，vidsRepresents video stream，audsRepresents audio stream
    uint32_t fourcc_codec;         //Specify the decoder required to process this stream, such as JPEG
    uint32_t flags;             //Flags, such as whether to allow this stream output, whether the palette changes, etc., generally set to 0
    uint16_t priority;        //stream priority，视频流Just set it to 0
    uint16_t language;        //audio language code，视频流Just set it to 0
    uint32_t init_frames;       //Specifies the initial frame number for interactive formats(Non-interactive format should be specified as0)
    uint32_t scale;             //
    uint32_t rate;              //For video streaming，rate / scale = Frame ratefps
    uint32_t start;             //For video streaming，Just set it to 0
    uint32_t length;            //For video streaming，length即total frames
    uint32_t suggest_buff_size; //Recommended buffer size for reading this stream data
    uint32_t quality;           //Quality metrics for streaming data
    uint32_t sample_size;       //audio sample size，视频流Just set it to 0
    AVI_RECT_FRAME rcFrame;         //The display position of this stream in the main video window，set to{0,0，width,height}That’s it
}AVI_STRH_CHUNK;

/*For video streaming，strfpiece结构like下*/
typedef struct {
    uint32_t FourCC;             //pieceID，fixed tostrf
    uint32_t size;               //piece大小，wait于struct avi_strf_chunkremoveidandsizesize
    uint32_t size1;              //size1含义and值同sizeSame
    uint32_t width;              //Video main window width (unit: pixels)
    uint32_t height;             //Main video window height (unit: pixels)
    uint16_t planes;             //always 1  
    uint16_t bitcount;           //Number of bits per pixel，can only be1、4、8、16、24and32one of the
    uint32_t fourcc_compression; //Video stream encoding format，like"JPEG"、"MJPG"wait
    uint32_t image_size;         //Video image size，wait于width * height * bitcount / 8
    uint32_t x_pixels_per_meter; //The horizontal resolution of the display device，Just set it to 0
    uint32_t y_pixels_per_meter; //The vertical resolution of the display device，Just set it to 0
    uint32_t num_colors;         //The meaning is unclear，Just set it to 0   
    uint32_t imp_colors;         //The meaning is unclear，Just set it to 0
}AVI_VIDS_STRF_CHUNK;

/*For audio streaming，strfpiece结构like下*/
typedef struct __attribute__((packed)) {
    uint32_t FourCC;             //pieceID，fixed tostrf
    uint32_t size;               //piece大小，wait于struct avi_strf_chunkremoveidandsizesize
    uint16_t format_tag;
    uint16_t channels;
    uint32_t samples_per_sec;
    uint32_t avg_bytes_per_sec;
    uint16_t block_align;
    uint32_t bits_per_sample;
}AVI_AUDS_STRF_CHUNK;

typedef struct 
{
    AVI_LIST_HEAD strl;
    AVI_STRH_CHUNK strh;
    AVI_VIDS_STRF_CHUNK strf;
}AVI_STRL_LIST;

typedef struct 
{
    AVI_LIST_HEAD hdrl;
    AVI_AVIH_CHUNK avih;
    AVI_STRL_LIST  strl;
}AVI_HDRL_LIST;

typedef struct {
    uint32_t FourCC; //Block ID, fixed to "idx1"
    uint32_t flags;
    uint32_t chunkoffset;
    uint32_t chunklength;
}AVI_IDX1;

/**
"db"：uncompressed video frames（RGBdata flow）；
"dc"：Compressed video frames；
"wb"：audio uncompressed data（Wavedata flow）；
"wc"：audio compressed data（compressedWavedata flow）；
"pc"：Switch to a new color palette。（The new palette uses a data structureAVIPALCHANGEto define。like果一个流的调色板中途可能改变，then it should be in the description of this stream format，That isAVISTREAMHEADERstructuraldwFlagscontains aAVISF_VIDEO_PALCHANGESmark。）
*/

#endif