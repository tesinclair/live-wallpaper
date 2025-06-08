#pragma once

#include <fstream>
#include <string>
#include <cstdint>
#include <cstddef>
#include <assert.h>
#include <algorithm>
#include <filesystem>
#include <cstring>

//Debug
#include <iostream>
#include <typeinfo>

#include "customErr.hpp"

// ATOM_EXT mean the extenstion after type
#define ATOM_READ_LENGTH 8 
#define MAX_VIDEO_LENGTH 60

struct atom{
    uint32_t size;
    uint32_t type;
    unsigned long long int size64;
    char typeName[4]; // bytes pointer
}__attribute__((packed));

struct test{
    uint32_t size;
    uint32_t type;
};

struct videoTrack{
    uint8_t version;
    char matrixStructure[36];
    uint16_t width;
    uint16_t height;
    uint32_t timescale;
    uint64_t duration; // In movie timescale
    uint64_t durationSeconds;
};

struct sampleTable{
    char codec[4];
    uint16_t frameCount;
    uint32_t numEntries;
    uint32_t sampleDuration;
    uint32_t sampleCount;
    char currentChunk[12];
    size_t nextChunk; // location of the next Sample in the table
};

class Reader{
public: // Functions
    /// @Param fname: the name of the file to read
    ///                 - Should be .mp4
    Reader (char* filename);   
    /// @Purpose ensures the opened filestream is closed.
    ~Reader();

    /// @Purpose gathers the video metadata and fills the
    ///          video track data. Also validates video length
    void getVideoData(videoTrack& vTrack);

    /// @Returns videoTrack.
    /// @Purpose reads and outputs the video data from the mp4 file
    char* getCurrentFrame();

    /// @Returns the current state of isEOF
    bool get_isEOF();

    /// @Returns the code used
    char* getCodec();

    /// @Returns uint32_t containing height:width, uint16_t:uint16_t
    uint32_t getScreenDimensions();

public: // Variables
    err readError;

private: // Functions
    /// @Purpose closes the ifstream safely
    void closeInfile();

    /// @Param an offset from the current position to the wanted data
    /// @Returns 4 bytes of data at that position
    /// @Purpose gets data from inside atom at offset 
    ///       while preserving the current position
    uint64_t getAtomData(size_t offset, size_t byte = 4);

    void getLongAtomData(size_t offset, size_t bytes, char* data);

    /// @Param an optional offset representing the start
    ///        of the desired atom from the start of the file
    /// @Purpose fills up the currentAtom field with the next atom
    ///       data or the next atom at the given offset
    void getNextAtom(size_t offset = 0);

    size_t moveToNextAtom(bool isNested = false, size_t offset = 0, bool fromStart = false);

    /// @Param either 4 or 8 bytes of data to reverse endianness
    /// @Purpose reverses endianness by moving each byte from the back 
    ///       to the front. 
    err reverseEndian(char* bytes, uint8_t size = 4);

private: // Variables
    char* filename;
    std::filesystem::path filepath;
    std::ifstream infile;
    atom currentAtom;
    bool isEOF;
    char* currentFrame;
    char codec[4];
    sampleTable frameData;
};
