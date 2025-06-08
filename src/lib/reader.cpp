#include "reader.hpp"

//Public 
Reader::Reader(char* filename) : 
            isEOF(false), readError{.err = false, .errMsg = ""}{

    this->filepath = std::filesystem::current_path() / filename;

    infile = std::ifstream(this->filepath, std::ios::binary); 

    if (!infile.is_open()){
        readError.err = true;
        readError.errMsg = "file could not be opened";
    }
}

Reader::~Reader(){
    closeInfile();
}

void Reader::getVideoData(videoTrack& vTrack){
    size_t curpos = 0;
    getNextAtom();
    while (strncmp(currentAtom.typeName, "moov", 4) != 0){
        curpos += moveToNextAtom();
        getNextAtom();
    }

    curpos += moveToNextAtom(true);
    getNextAtom();
    if (strncmp(currentAtom.typeName, "mvhd", 4) != 0){
        readError.err = true;
        readError.errMsg = "Couldn't find movie header";
        return;
    }
    
    uint32_t timescale = getAtomData(20); // 20 bytes is timescale
    vTrack.timescale = timescale;                                                             

    curpos += moveToNextAtom(); // Go into trak
    curpos += moveToNextAtom(true); // go into tkhd
    getNextAtom(); // get size of tkhd

    // Stop updating curpos to allow return to tkhd
    moveToNextAtom(); // go into mdia
    getNextAtom(); // get size of mdia
    if (strncmp(currentAtom.typeName, "edts", 4) == 0){ // check for edit header
        moveToNextAtom(true);
        getNextAtom();
        moveToNextAtom();
        getNextAtom();
    }

    assert(strncmp(currentAtom.typeName, "mdia", 4) == 0);

    moveToNextAtom(true); // go into mdhd
    getNextAtom(); // get size of mdia
    moveToNextAtom(); // go into hdlr
    getNextAtom(); // get size of mdia

    if (strncmp(currentAtom.typeName, "hdlr", 4) != 0){
        readError.err = true;
        readError.errMsg = "Couldn't find handler reference atom";
        return;
    }

    uint32_t hdlrSubtypeInt = getAtomData(16);
    if (readError.err){
        return;
    }
    char hdlrSubtype[4];
    for (int i = 0; i < 4; ++i){
        hdlrSubtype[i] = reinterpret_cast<char*>(&hdlrSubtypeInt)[3-i];
    }

    if (strncmp(hdlrSubtype, "vide", 4) != 0){ // 16 bytes is component subtype
        readError.err = true;
        readError.errMsg = "Only Accepting video-first mp4 files";
        return;
    }
    
    moveToNextAtom(false, curpos, true); // return to tkhd
    getNextAtom();
    assert(strncmp(currentAtom.typeName, "tkhd", 4) == 0); /// @DEBUG

    vTrack.version = static_cast<uint8_t>(getAtomData(8, 1));
    uint64_t duration;
    if (vTrack.version == 1)
        duration = getAtomData(28, 8);
    else
        duration = getAtomData(28); // 4 bytes for small
    
    vTrack.duration = duration;

    vTrack.durationSeconds = vTrack.duration / vTrack.timescale;

    char matrix[36];
    getLongAtomData(52, 36, &matrix[0]);
    if (readError.err){
        return; // Shit the bed
    }

    std::copy(matrix, matrix + 36, vTrack.matrixStructure);

    uint16_t width;
    uint16_t height;

    if (vTrack.version == 0){
        width = static_cast<uint16_t>(getAtomData(82));
        height = static_cast<uint16_t>(getAtomData(86));
    }else{
        width = static_cast<uint16_t>(getAtomData(94));
        height = static_cast<uint16_t>(getAtomData(98));
    }

    if (readError.err){
        return;
    }
    vTrack.height = height;
    vTrack.width = width;

    return;

    // Now to get the actual real data.
}


char* Reader::getCurrentFrame(){
    // uses the 
    return currentFrame;
}

bool Reader::get_isEOF(){
    return isEOF;
}

char* Reader::getCodec(){
    return codec;
}

// Private
void Reader::closeInfile(){
    infile.close();
}

uint64_t Reader::getAtomData(size_t offset, size_t bytes){
    uint64_t data;

    infile.seekg(offset, std::ios::cur);

    if (!infile.read(reinterpret_cast<char*>(&data), bytes)){
        readError.err = true;
        readError.errMsg = "Could not get atom data";
    }

    if (reverseEndian(reinterpret_cast<char*>(&data)).err){
        readError.err = true;
        readError.errMsg = "Could not change endianness";
    }

    infile.seekg(-(bytes + offset), std::ios::cur);

    return data;
}

void Reader::getLongAtomData(size_t offset, size_t bytes, char* data){
    infile.seekg(offset, std::ios::cur);

    if (!infile.read(data, bytes)){
        readError.err = true;
        readError.errMsg = "Could not read long atom data";
    }

    if (reverseEndian(data).err){
        readError.err = true;
        readError.errMsg = "Could not change endianness";
    }

    infile.seekg(-(bytes + offset), std::ios::cur);
}

void Reader::getNextAtom(size_t offset){
    size_t curpos = infile.tellg();
    if (offset != 0){
        infile.seekg(offset, std::ios::beg);
    }
    
    if (!infile.read(reinterpret_cast<char*>(&currentAtom), ATOM_READ_LENGTH)){
        isEOF = true;
        return;
    }

    reverseEndian(reinterpret_cast<char*>(&currentAtom.size));

    if (currentAtom.size == 1){
        // then using 64 bit sizes
        if (!infile.read(reinterpret_cast<char*>(&currentAtom.size64), sizeof(currentAtom.size64))){
            isEOF = true;
            return;
        }
        // move back cursor
        infile.seekg(curpos);
    }

    for (int i = 0; i < sizeof(currentAtom.typeName); ++i){
        currentAtom.typeName[i] = reinterpret_cast<char*>(&currentAtom.type)[i];
    }

    // move back cursor to start of atom
    infile.seekg(curpos);
}

size_t Reader::moveToNextAtom(bool isNested, size_t offset, bool fromStart){
    if (offset != 0){
        if (fromStart){
            infile.seekg(offset, std::ios::beg);
            return offset;
        }
        infile.seekg(offset, std::ios::cur);
        return offset;
    }
    if (isNested){
        infile.seekg(8, std::ios::cur);
        return 8;
    }
    if (currentAtom.size == 1){
        infile.seekg(currentAtom.size64, std::ios::cur);
        return currentAtom.size64;
    }

    infile.seekg(currentAtom.size, std::ios::cur);
    return currentAtom.size;
}

err Reader::reverseEndian(char* bytes, uint8_t size){
    if (bytes == nullptr){
        err endianError = {.err = true, .errMsg = "Reverse Endian Err: Bytes were nullptr"};
        return endianError;
    }
    if (size == 2){
        char tempBytes[2];
        for (int i = 0; i < sizeof(tempBytes); ++i){
            tempBytes[i] = bytes[1 - i];
        }
        for(int i = 0; i < sizeof(tempBytes); ++i){
            bytes[i] = tempBytes[i];
        }
    }else if (size == 4){
        char tempBytes[4];
        for (int i = 0; i < sizeof(tempBytes); ++i){
            tempBytes[i] = bytes[3 - i];
        }
        for(int i = 0; i < sizeof(tempBytes); ++i){
            bytes[i] = tempBytes[i];
        }
    }else if (size == 8){
        char tempBytes[8];
        for (int i = 0; i < sizeof(tempBytes); ++i){
            tempBytes[i] = bytes[7 - i];
        }
        for(int i = 0; i < sizeof(tempBytes); ++i){
            bytes[i] = tempBytes[i];
        }
    }else{
        err endianError = {.err = true, .errMsg = "Reverse Endian Err: Can only reverse 2, 4 or 8 bytes"};
    }

    err endianError = {.err = false, .errMsg = ""};
    return endianError;
}
