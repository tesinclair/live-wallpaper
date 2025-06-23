#pragma once

//FFmpeg
extern "C"{

#include <libavformat/avformat.h>
//#include <libavutil/imgutils.h>
//#include <libavutil/avutil.h>
//#include <libswscale/swscale.h>

}

#include <filesystem>

#include "shared/lw_err.hpp"
#include "shared/lw_codec.hpp"
#include "shared/lw_types.hpp"

class Reader{
public: // Functions
    /**
     * @brief Constructor for a reader object.
     * @param filename This should be a direct path to the file
     *                 It is not the responsibility of the constructor
     *                 to locate the file.
     * @details Only returns a reader object. All initialization
     *          of the reader (including setting up the decoder)
     *          should be done in Reader::init();
     */
    Reader(std::filesystem::path filename);

    ~Reader();

    /**
     * @brief initialises the reader object.
     * @details Does all the initialisation for the reader
     *          including creating the Decoder, the av contexts
     *          and more.
     */
    void init();

    /**
     * @brief gets the next frame in the file (decoded)
     */
    AVPacket* getNextPacket();

    bool get_isEOF();

public: // variables
    err readerError = {"Reader::none", false, NOERR, ""};

private: // Variables
    std::filesystem::path m_filename;
    Decoder m_decoder;
    AVPacket* m_pkt = nullptr;
    AVFormatContext* m_fmtctx = nullptr;
    AVFrame* m_frame = nullptr;
    bool m_isEOF = false;
    
    size_t m_frameIsVFrame = false;
};

