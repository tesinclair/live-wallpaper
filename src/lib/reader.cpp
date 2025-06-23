#include "reader.hpp"

// Public:

Reader::Reader(std::filesystem::path filename) :
    m_filename(filename){
    LOG_INFO("Reader:Reader", "Creating the reader object...");
    }

~Reader(){
    LOG_INFO("Reader:Reader", "Destroying the reader object...");
    avformat_close_input(&m_fmtctx);
    av_packet_free(&m_pkt);
    av_frame_free(&m_frame);
    LOG_INFO("Reader:Reader", "Freed FFmpeg memory");
}

void Reader::init(){
    LOG_INFO("Reader:init", "Initialising reader...");
    LOG_INFO("Reader:init", "Opening input file...");
    if (avformat_open_input(&m_fmtctx, m_filename.string().c_str(), nullptr, nullptr) < 0){
        LOG_WARN("Reader:init", "Failed to open the input file. Expect an error");
        readerError = {"Reader:init", true, EAVOI, "Failed to open the input file"};
        return;
    }
    LOG_INFO("Reader:init", "Successfully opened the input file.");

    LOG_INFO("Reader:init", "Getting the video stream info...");
    if (avformat_find_stream_info(m_fmtctx, nullptr) < 0){
        LOG_WARN("Reader:init", "Failed to get video stream info. Expect an error.");
        readerError = {"Reader:init", true, EAVFSI, "Failed to find stream info"};
        return;
    }
    LOG_INFO("Reader:init", "Successfully read the video stream info.");

    LOG_INFO("Reader:init", "Initialising the decoder...");
    m_decoder.init(m_fmtctx);
    if (m_decoder.decoderError.err){
        LOG_WARN("Reader:init", "Failed to initialise the decoder. Expect an error.");
        readerError = m_decoder.decoderError;
        return;
    }
    LOG_INFO("Reader:init", "Successfully initialised the decoder.");

#if defined(DEBUG) || defined(VERBOSE)
    LOG_INFO("Reader:init", "Dumping stream info...");
    av_dump_format(m_fmtctx, 0, m_filename.string().c_str(), 0);
    LOG_INFO("Reader:init", "See stream info above.");
#endif
    
    LOG_INFO("Reader:init", "Allocating frame memory...");
    m_frame = av_frame_alloc();
    if (m_frame == nullptr){
        LOG_WARN("Reader:init", "Failed to allocate frame memory. Expect an error.");
        readerError = {"Reader:init", true, EFFA, "Failed to allocate frame"};
        return;
    }
    LOG_INFO("Reader:init", "Allocated frame memory.");

    LOG_INFO("Reader:init", "Allocating packet memory...");
    m_pkt = av_packet_alloc();
    if (m_pkt == nullptr){
        LOG_WARN("Reader:init", "Failed to allocate packet memory. Expect an error.");
        readerError = {"Reader:init", true, EFPA, "Failed to allocate packet"};
        return;
    }
    LOG_INFO("Reader:init", "Allocated packet memory.");

    LOG_INFO("Reader:init", "Successfully initialised Reader object. Returning...");
}

AVPacket* Reader::getNextPacket(){
    LOG_INFO("Reader:getNextPacket", "Getting the next packet...");

    while (!m_frameIsVFrame){
        if ((int ret = av_read_frame(m_fmtctx, m_pkt)) < 0){
            if (ret == AVERROR_EOF){
                LOG_WARN("Reader:getNextPacket", "End of file reached.");
                m_isEOF = true;
                return nullptr;
            }
            LOG_WARN("Reader:getNextPacket", "Failed to read the packet. Expect an error.");
            readerError = {"Reader:getNextPacket", true, ERDFRM, "Failed to read the next packet"};
            return nullptr;
        }
        LOG_INFO("Reader:getNextPacket", "Got the next packet");

        LOG_INFO("Reader:getNextPacket", "Checking if packet is video stream");
        if (m_pkt->stream_index == m_decoder.videoStreamIdx){
            m_frameIsVFrame = true;
            LOG_INFO("Reader:getNextPacket", "Packet was a video stream. Decoding...");
            m_decoder.decode(m_pkt);
            if (m_decoder.decoderError.err){
                LOG_WARN("Reader:getNextPacket", "Failed to decode the packet. Expect an error.");
                av_packet_unref(m_pkt);
                readerError = m_decoder.decoderError;
                return nullptr;
            }
            av_packet_unref(m_pkt);
            LOG_INFO("Reader:getNextPacket", "Decoded packet...");
        }
    }

    LOG_INFO("Reader:getNextPacket", "Decoded Video Frame. Returning it...");

    return m_pkt;
}

bool Reader::get_isEOF(){
    return m_isEOF;
}

