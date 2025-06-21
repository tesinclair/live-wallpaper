#pragma once

#include "shared/lw_err.hpp"
#include "shared/lw_codec.hpp"
#include "shared/lw_types.hpp"

class Decoder{
public: // Functions
    Decoder(enum Codec encoding);

public: // variables
    err decodeError = {.err = false, .errMsg = ""};

private: // Functions

private: // Variables
    enum Codec _encoding;
    frame _rawFrame;
    frame _decodedFrame;
}
