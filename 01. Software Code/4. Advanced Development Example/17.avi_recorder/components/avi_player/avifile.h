#ifndef __AVIFILE_H
#define __AVIFILE_H

#include "avi_def.h"

#define DEBUGINFO	//Information print switch

/** big_endian */
// #define RIFF_ID		0x52494646
// #define AVI_ID		0x41564920
// #define LIST_ID		0x4c495354
// #define hdrl_ID		0x6864726c
// #define avih_ID		0x61766968
// #define strl_ID		0x7374726c
// #define strh_ID		0x73747268
// #define strf_ID		0x73747266
// #define movi_ID		0x6d6f7669
// #define mjpg_ID     0x4D4A5047
// #define vids_ID     0x76696473
// #define auds_ID     0x61756473

/** little_endian */
#define RIFF_ID		_REV(0x52494646)
#define AVI_ID		_REV(0x41564920)
#define LIST_ID		_REV(0x4c495354)
#define hdrl_ID		_REV(0x6864726c)
#define avih_ID		_REV(0x61766968)
#define strl_ID		_REV(0x7374726c)
#define strh_ID		_REV(0x73747268)
#define strf_ID		_REV(0x73747266)
#define movi_ID		_REV(0x6d6f7669)
#define mjpg_ID     _REV(0x4D4A5047)
#define vids_ID     _REV(0x76696473)
#define auds_ID     _REV(0x61756473)

typedef struct
{	
      uint32_t  RIFFchunksize;
      uint32_t  LISTchunksize;
    uint32_t  avihsize;
    uint32_t  strlsize;
    uint32_t  strhsize;

    uint32_t movi_start; //The starting position of the data block
    uint32_t movi_size;

    uint16_t vids_fps;
    uint16_t vids_width;
    uint16_t vids_height;

    uint16_t auds_channels;
    uint16_t auds_sample_rate;
    uint16_t auds_bits;

}AVI_TypeDef;

// typedef struct
// {	
// 	uint32_t SecPerFrame; //Display the time of each frame
// 	uint32_t  MaxByteSec; 	//maximum data transfer rate
// 	uint32_t ChunkBase; //The block length is a multiple of this value, usually 2048
// 	uint32_t  SpecProp;	  //AVISpecial properties of files
// 	uint32_t TotalFrame; //The total number of frames in the file
// 	uint32_t  InitFrames;  //Number of initial frames before starting playback
// 	uint32_t  Streams;		  //Types of data streams included
// 	uint32_t RefBufSize; //Recommended buffer size, usually the sum of storing one frame of image and synchronized audio data
// 	uint32_t  Width;		    //image width
// 	uint32_t  Height;		  //image height
// 	uint32_t  Reserved;	  //reserve
// }avih_TypeDef;

// typedef struct
// {	
// 	uint8_t StreamType[4];//Data flow type，vids(0x73646976):video,auds:Audio
// 	uint8_t Handler[4]; //Driver code name for data decompression
// 	uint32_t   StreamFlag;   //Data flow properties
// 	uint32_t    Priority;	    //The priority of this data stream playback
// 	uint32_t Language; //Audio language code
// 	uint32_t   InitFrames;   //Number of initial frames before starting playback
// 	uint32_t Scale; //The size of each frame of the video or the sampling rate of the audio
// 	uint32_t   Rate; 		    //Scale/Rate=samples per second
// 	uint32_t   Start;		    //The position at which the data stream starts playing，The unit isScale
// 	uint32_t   Length;		    //The amount of data in the data stream，The unit isScale
//  	uint32_t   RefBufSize;   //Recommended buffer size
//   uint32_t   Quality;		  //Decompression quality parameters，The larger the value，The better the quality
// 	uint32_t   SampleSize;	  //audio sample size
// 	struct{				
// 	  short int  Left;
// 		short int  Top;
// 		short int  Right;
// 		short int  Bottom;
// 	}Frame; //The rectangle occupied by the video frame 
// }strh_TypeDef;

// typedef struct tagBMPHEADER
// {
//   uint32_t  	Size;		      //The number of bytes occupied by this structure
// 	long   	Width;		    //image width
// 	long   	Height;		    //image height
// 	uint32_t Planes; //Plane number, must be 1
// 	uint32_t   	BitCount;	    //Number of pixels
// 	uint8_t  	Compression[4];	//Compression type，MJPEG
// 	uint32_t SizeImage; //Image size
// 	long   	XpixPerMeter; //horizontal resolution
// 	long   	YpixPerMeter; //vertical resolution
// 	uint32_t  	ClrUsed;		  //The actual number of colors in the palette used,Not used in compressed formats
// 	uint32_t ClrImportant; //Important colors
// }BMPHEADER;             //This structure occupies40byte

// typedef struct tagRGBQUAD 
// {
// 	uint8_t rgbBlue; //Brightness of blue (value range is 0-255)
// 	uint8_t  rgbGreen;       //green brightness(The value range is0-255)
// 	uint8_t  rgbRed; 	      //brightness of red(The value range is0-255)
// 	uint8_t rgbReserved; //Reserved, must be 0
// }RGBQUAD; //Color table

// typedef struct tagBITMAPINFO 
// {
// 	BMPHEADER bmiHeader; //Bitmap information header
// 	RGBQUAD bmColors[1];//Color table
// }BITMAPINFO;  

// typedef struct 
// {
//   uint32_t  FormatTag;
// 	uint32_t  Channels;	  //Number of channels
// 	uint32_t SampleRate; //sampling rate
// 	uint32_t BaudRate; //Baud rate
// 	uint32_t  BlockAlign; //Data block alignment flag
// 	uint32_t Size; //Size of this structure
// }WAVEFORMAT;

#define	 MAKEWORD(ptr)	(uint32_t)(((uint32_t)*((uint8_t*)(ptr))<<8)|(uint32_t)*(uint8_t*)((ptr)+1))
#define  MAKEuint32_t(ptr)	(uint32_t)(((uint32_t)*(uint8_t*)(ptr)|(((uint32_t)*(uint8_t*)(ptr+1))<<8)|\
                        (((uint32_t)*(uint8_t*)(ptr+2))<<16)|(((uint32_t)*(uint8_t*)(ptr+3))<<24))) 

int AVI_Parser(const uint8_t *buffer, uint32_t length);

#endif