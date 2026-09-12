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
    uint32_t size;   //Chunk size, equal to the size of the subsequent data
    /* data uint8_t _data[size]; */
}AVI_CHUNK_HEAD;

typedef struct {
    uint32_t List;   //Fixed to "LIST"; for a RIFF list it is "RIFF"
    uint32_t size;   //Chunk size, equal to the size of the subsequent data
    uint32_t FourCC;   
    /* Data uint8_t _data[size-4]; */
}AVI_LIST_HEAD;

typedef struct {
    uint32_t FourCC;            //Block ID, fixed to avih
    uint32_t size;              //Block size, equal to the size of struct avi_avih_chunk minus id and size
    uint32_t us_per_frame;      //Video frame interval time in microseconds
    uint32_t max_bytes_per_sec; //Maximum data rate of the AVI file
    uint32_t padding;           //Just set it to 0
    uint32_t flags;             //AVI file global attributes, such as whether it contains index blocks, whether audio and video data are cross-stored, etc.
    uint32_t total_frames;      //total frames
    uint32_t init_frames;       //Initial frame number for interactive formats (set to 0 for non-interactive formats)
    uint32_t streams;           //Number of streams contained in the file; set to 1 when there is only a video stream
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
    uint32_t FourCC;            //Chunk ID, fixed to strh
    uint32_t size;              //Chunk size, equal to sizeof(struct avi_strh_chunk) minus id and size
    uint32_t fourcc_type;   //Stream type: vids for a video stream, auds for an audio stream
    uint32_t fourcc_codec;         //Specify the decoder required to process this stream, such as JPEG
    uint32_t flags;             //Flags, such as whether to allow this stream output, whether the palette changes, etc., generally set to 0
    uint16_t priority;        //Stream priority; set to 0 for a video stream
    uint16_t language;        //Audio language code; set to 0 for a video stream
    uint32_t init_frames;       //Initial frame number for interactive formats (set to 0 for non-interactive formats)
    uint32_t scale;             //
    uint32_t rate;              //For a video stream, rate / scale = frame rate (fps)
    uint32_t start;             //For a video stream, set it to 0
    uint32_t length;            //For a video stream, length is the total number of frames
    uint32_t suggest_buff_size; //Recommended buffer size for reading this stream data
    uint32_t quality;           //Quality metrics for streaming data
    uint32_t sample_size;       //Audio sample size; set to 0 for a video stream
    AVI_RECT_FRAME rcFrame;         //Display position of this stream in the main video window; set to {0, 0, width, height}
}AVI_STRH_CHUNK;

/*For a video stream, the strf chunk structure is as follows*/
typedef struct {
    uint32_t FourCC;             //Chunk ID, fixed to strf
    uint32_t size;               //Chunk size, equal to sizeof(struct avi_strf_chunk) minus id and size
    uint32_t size1;              //size1 has the same meaning and value as size
    uint32_t width;              //Video main window width (unit: pixels)
    uint32_t height;             //Main video window height (unit: pixels)
    uint16_t planes;             //always 1  
    uint16_t bitcount;           //Number of bits per pixel; can only be one of 1, 4, 8, 16, 24, 32
    uint32_t fourcc_compression; //Video stream encoding format, such as "JPEG", "MJPG", etc.
    uint32_t image_size;         //Video image size, equal to width * height * bitcount / 8
    uint32_t x_pixels_per_meter; //Horizontal resolution of the display device; set to 0
    uint32_t y_pixels_per_meter; //Vertical resolution of the display device; set to 0
    uint32_t num_colors;         //Meaning unclear; set to 0   
    uint32_t imp_colors;         //Meaning unclear; set to 0
}AVI_VIDS_STRF_CHUNK;

/*For an audio stream, the strf chunk structure is as follows*/
typedef struct __attribute__((packed)) {
    uint32_t FourCC;             //Chunk ID, fixed to strf
    uint32_t size;               //Chunk size, equal to sizeof(struct avi_strf_chunk) minus id and size
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
"db": uncompressed video frames (RGB data stream);
"dc": compressed video frames;
"wb": uncompressed audio data (WAV data stream);
"wc": compressed audio data (compressed WAV data stream);
"pc": Switch to a new color palette. (The new palette is defined using the AVIPALCHANGE data structure. If a stream's palette may change partway through, it should be described in that stream's format description, i.e. the dwFlags field of the AVISTREAMHEADER structure contains the AVISF_VIDEO_PALCHANGES flag.)
*/

#endif