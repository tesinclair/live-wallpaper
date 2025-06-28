#pragma once

#include "shared/lw_err.hpp"
#include "shared/lw_codec.hpp"
#include "shared/lw_types.hpp"

class Decoder{
public: // Functions
    Decoder();
    ~Decoder();

    void init();

    /**
     *  @brief Getter for member variable videoStreamIndex
     */
    int get_videoStreamIndex();

    /**
     * @brief Fills an array of frames with decoded frames
     *        from a given packet
     * @param pkt The packet containing the frame data
     * @param frames a pointer to an array of empty frames
     *               to fill.
     * @param frameCount the number of frames to fill. Will fill
     *                   until either there are no more frames or 
     *                   frameCount has been reached (which ever
     *                   comes first).
     */
    void decode(AVPacket* pkt, AVFrame* frames[], size_t frameCount);

public: // variables
    err decoderError = {"Decoder", false, NOERR, ""};

private: // Functions

private: // Variables
    int m_videoStreamIdx;
    AVCodecContext* m_codecCtx = nullptr;
    AVFrame* m_frame = nullptr;
    AVCodec* m_codec;
    AVCodecParserContext* m_parserCtx;
}
