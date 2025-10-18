/**
 ****************************************************************************************************
 * @file        spi_sdcard.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       SDCARD Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#include "spi_sdcard.h"
#include <SD.h>
#include <SPI.h>
#include "myfs.h"

File myFile;
SPIClass spi_sdcard;
 
/**
 * @brief       initializationSDCard
 * @param       none
 * @retval      Return value:0 initializationcorrect；other values，initializationmistake
 */
uint8_t sdcard_init(void)
{
    spi_sdcard.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);    /* Set the pins used for SPI of the SD card */ 

    pinMode(SD_CS_PIN, OUTPUT);                 /* Chip select pinset upfor output */

    if (!SD.begin(SD_CS_PIN, spi_sdcard))       /* SDCardinitialization */
    {
        Serial.println("SDCardinitializationfail!");

        if (!SD.begin(SD_CS_PIN, spi_sdcard))   /* SD card initialization failed and initialized again */
        {
            Serial.println("SDCardinitializationfail!");
            return 1;
        }
    }
    Serial.println("SDCardinitializationsuccess");

    uint8_t cardType = SD.cardType();   /* Get card type */
    if (cardType == CARD_NONE)          /* Memory card not connected */
    {
        Serial.println("NoCardconnect");
        return 2;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)           /* mmc card */
    {
        Serial.println("MMC");
    } 
    else if (cardType == CARD_SD)       /* sd card, maximum 2G */
    {
        Serial.println("SDSC");
    } 
    else if (cardType == CARD_SDHC)     /* sdhcCard，maximum32G */
    {
        Serial.println("SDHC");
    } 
    else                                /* unknown memory card */
    {
        Serial.println("UNKNOWN");
    }
    return 0;
}

/**
 * @brief       SDCard test code
 * @param       none
 * @retval      none
 */
void sd_test(void)
{
    myFile = SD.open("/test.txt", FILE_WRITE);    /* Open create file */
    if (myFile)                                   /* The file was opened successfully */
    {
        Serial.print("Writing to test.txt...");
        myFile.println("testing 1, 2, 3.");       /* Write data into a file */

        myFile.close();                           /* close file */
        Serial.println("done.");
    } 
    else                                          /* File opening failed */
    {
        Serial.println("error opening test.txt");
    }

    myFile = SD.open("/test.txt");               /* Open the previously operated file */
    if (myFile) 
    {
        Serial.println("test.txt:");

        while (myFile.available())                  
        {
            Serial.write(myFile.read());          /* Read all contents in the file */
        }

        myFile.close();                           /* close file */
    } 
    else                                          /* File opening failed */
    {
        Serial.println("error opening test.txt");
    }

    listDir(SD, "/", 0);                          /* Traverse files in the directory */
    createDir(SD, "/mydir");                      /* Create folder */
    listDir(SD, "/", 0);                          /* Traverse files in the directory */
    removeDir(SD, "/mydir");                      /* Remove folder */
    listDir(SD, "/", 2);                          /* Traverse files in the directory */
    writeFile(SD, "/hello.txt", "Hello ");        /* Write data into a file */
    appendFile(SD, "/hello.txt", "World!\n");     /* Append data after file */
    readFile(SD, "/hello.txt");                   /* Read data from file */
    deleteFile(SD, "/foo.txt");                   /* Delete files */
    renameFile(SD, "/hello.txt", "/foo.txt");     /* Modify file name */
    readFile(SD, "/foo.txt");                     /* Read data from file */ 
    testFileIO(SD, "/test.txt");                  /* Test file IO performance */
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));  /* Print SD card file system total capacity size */
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));    /* PrintSDThe size of the card file system has been used */
}