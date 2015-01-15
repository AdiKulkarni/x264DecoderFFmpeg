#ifndef X264_RTSP_H
#define X264_RTSP_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include <libswscale/swscale.h>
}

class rtsp {

private:

    AVFormatContext* context;
    int video_stream_index;

public:

    rtsp(AVFormatContext* cnt);
    int init(std::string ipAddr, int port);
    AVFormatContext* getRTSPContext();
    int getVideoStreamIndex();
    void close();

};

#endif
