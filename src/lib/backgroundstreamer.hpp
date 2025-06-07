#pragma once

#include "decoder.hpp"

class BackgroundStreamer {
public:
    BackgroundStreamer(Decoder decoder) : decoder(decoder);

    struct err stream();

private:
    getNextFrame();
}
