#pragma once

#include "shared/lw_err.hpp"
#include "shared/lw_codec.hpp"
#include "shared/lw_types.hpp"

class Decoder{
public: // Functions
    Decoder(enum Codec encoding);

    frame decodeFrame();

public: // variables
    err decodeError = {.err = false, .errMsg = ""};

private: // Functions
    // For now this is the only one I am using
    // I intend to do the rest at somepoint, but who knows
    void decodeAVC();

    void decodeAV1();
    void decodeHEVC();
    void decodeMPEG_2();
    void decodeMP4V_ES();
    void decodeVP9();

private: // Variables
    enum Codec _encoding;
    frame _rawFrame;
    frame _decodedFrame;
}
