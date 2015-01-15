//frameEncoder

#ifndef X264_EXAMPLE_DECODER_H
#define X264_EXAMPLE_DECODER_H

#include <inttypes.h>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <cstring>
#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "media_player.h"

extern "C" {
#include <x264.h>
}

namespace pt = boost::posix_time;
#define INBUF_SIZE 4096

class decoder {

private:

    int frame_count;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
    AVPacket avpkt;
    AVCodecParserContext *parser;
    media_player mPlayer;
    AVCodec *codec;
    AVCodecContext *codecContext;
    long recordOldTime = 0;

public:

    decoder();
    void initialize();
    int decodeFrame(const char *data, int length);
    void close();
};

#endif
