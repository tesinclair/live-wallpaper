#include "decoder.hpp"

/// @Public
Decoder::Decoder(enum Codec encoding)
    : encoding(_encoding){
}

frame Decoder::decodeFrame(frame rawFrame){
    if (_encoding != AVC){
        decodeError.err = true;
        decodeError.errMsg = "Currently only AVC is implemented";
        return nullptr;
    }

    _rawFrame = rawFrame;
    
    decodeAVC();
    return _decodedFrame;
}

/// @Private
void Decoder::decodeAVC(){
         
}



// NOT IMPLEMENTED
void Decoder::decodeAV1(){
}

void Decoder::decodeHEVC(){
}

void Decoder::decodeMPEG_2(){
}

void Decoder::decodeMP4V_ES(){
}

void Decoder::decodeVP9(){
}

