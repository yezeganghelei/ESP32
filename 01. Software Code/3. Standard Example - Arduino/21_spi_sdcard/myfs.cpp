/**
 ******************************************************************************************************
 * @file myfs.cpp
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief document system operation code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************
 *@attention

 *Modification instructions
 * V1.0 20231201
 *First release

 ******************************************************************************************************
 */

#include "myfs.h"
#include <FS.h>
#include <SD.h>

/**
 * @brief       TraverseTable of contentsdowndocument
 * @param       fs : file object
 * @param       dirname : Table of contentspath
 * @param       levels : Traversal depth
 * @retval      none
 */
void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root)
    {
        Serial.println("Failed to open directory");
        return;
    }

    if(!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file)
    {
        if(file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels)
            {
                listDir(fs, file.path(), levels -1);
            }
        } 
        else 
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

/**
 * @brief       createdocumentfolder
 * @param       fs : file object
 * @param       path : needcreateofdocumentfolder name。It needs to include a path，For path'/'Separation，for example"/mydir"
 * @retval      none
 */
void createDir(fs::FS &fs, const char * path)
{
    Serial.printf("Creating Dir: %s\n", path);

    if(fs.mkdir(path))
    {
        Serial.println("Dir created");
    } 
    else 
    {
        Serial.println("mkdir failed");
    }
}

/**
 * @brief remove folder
 * @param fs: file object
 * @param path: The name of the folder to be removed. It needs to include the path, separated by '/', such as "/mydir"
 * @retval None
 */
void removeDir(fs::FS &fs, const char * path)
{
    Serial.printf("Removing Dir: %s\n", path);

    if(fs.rmdir(path))
    {
        Serial.println("Dir removed");
    } 
    else 
    {
        Serial.println("rmdir failed");
    }
}

/**
 * @brief       fromdocumentRead data in 
 * @param       fs : file object
 * @param       path : Files to be read。It needs to include a path，For path'/'Separation，for example"/test.txt"
 * @retval      none
 */
void readFile(fs::FS &fs, const char * path)
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

/**
 * @brief       Write data to file
 * @param       fs : file object
 * @param       path : Files to be written。It needs to include a path，For path'/'Separation，for example"/test.txt"
 * @retval      none
 */
void writeFile(fs::FS &fs, const char * path, const char * message)
{
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    if(file.print(message))
    {
        Serial.println("File written");
    } 
    else 
    {
        Serial.println("Write failed");
    }
    file.close();
}

/**
 * @brief       existdocumentAppend data later
 * @param       fs : file object
 * @param       path : Files to be written。It needs to include a path，For path'/'Separation，for example"/test.txt"
 * @param       message : Need to append data written to the file
 * @retval      none
 */
void appendFile(fs::FS &fs, const char * path, const char * message)
{
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file)
    {
        Serial.println("Failed to open file for appending");
        return;
    }

    if(file.print(message))
    {
        Serial.println("Message appended");
    } 
    else 
    {
        Serial.println("Append failed");
    }
    file.close();
}

/**
 * @brief Modify the file/Table of contentsname (if the Table of contents is different, you can also move the document!)
 * @param fs: file object
 * @param path1: previous name
 * @param path2: new name
 * @retval none
 */
void renameFile(fs::FS &fs, const char * path1, const char * path2)
{
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) 
    {
        Serial.println("File renamed");
    } 
    else 
    {
        Serial.println("Rename failed");
    }
}

/**
 * @brief Delete files/directories
 * @param fs : File object
 * @param path : file/directory path + name
 * @retval None
 */
void deleteFile(fs::FS &fs, const char * path)
{
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path))
    {
        Serial.println("File deleted");
    } 
    else 
    {
        Serial.println("Delete failed");
    }
}

/**
 * @brief       test fileIOperformance
 * @param       fs : file object
 * @param       path : document/Table of contentspath+name
 * @retval      none
 */
void testFileIO(fs::FS &fs, const char * path)
{
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;

    if(file)
    {
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len)
        {
            size_t toRead = len;
            if(toRead > 512)
            {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } 
    else 
    {
        Serial.println("Failed to open file for reading");
    }

    file = fs.open(path, FILE_WRITE);
    if(!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i = 0; i < 2048; i++)
    {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}