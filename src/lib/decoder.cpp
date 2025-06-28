#include "decoder.hpp"

/// @Public
Decoder::Decoder(){}

void Decoder::init(int codecID){
    m_codec = avcodec_find_decoder(codecID);
    m_codecCtx = avcodec_alloc_context3(m_codec);
    m_parserCtx = av_parser_init(m_codec->id);
}

void Decoder::~Decoder(){
    
}

void Decoder::decodePacket(AVPacket* pkt, AVFrame* frames[], size_t frameCount){
    size_t initialFrameCount = frameCount;
    int ret = avcodec_send_packet(m_codecCtx, pkt);
    if (ret < 0){
        LOG_WARN("Decoder:decodePacket", "Failed sending packet to decoder. Expect an error.");
        decoderError = {"Decoder:decodePacket", true, EFSPD, "Failed to send packet to decoder"};
        return;
    }

    while ((ret = avcodec_receive_frame(codecCtx, m_frame)) >= 0 && frameCount > 0){
        m_frame = av_frame_alloc();
        if (!m_frame){
            LOG_WARN("Decoder:decodePacket", "Failed to allocate frame on iteration %ld", initialFrameCount-frameCount + 1);
            decoderError = {"Decoder:decodePacket", true, EFAF, "Failed to allocate frame during iteration"};
            return;
        }
        LOG_INFO("Decoder:decodePacket", "Got frame %ld out of %ld. Putting it into frames at index: %ld", initialFrameCount - frameCount + 1, initialFrameCount, initialFrameCount - frameCount);
        frames[initialFrameCount - frameCount--] = m_frame;
        m_frame = nullptr;
    }

    LOG_INFO("Decoder:decodePacket", "Finished Reading frames with return value: %d", ret);
    LOG_INFO("Decoder:decodePacket", "Normal Codes: EAGAIN=%d, AVERROR_EOF=%d", AVERROR(EAGAIN), AVERROR_EOF);
    if (ret < 0 && !(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)){
        LOG_WARN("Decoder:decodePacket", "Failed to get frame from decoder. Expect an error.");
        decoderError = {"Decoder:decodePacket", true, EFRF, "Failed to receive frame from decoder"};
        return;
    }

    if (ret == AVERROR_EOF){
        LOG_WARN("Decoder:decodePacket", "End of file reached.");
        m_isEOF = true;
    }
}

int Decoder::get_videoStreamIdx(){
    return m_videoStreamIdx;
}
