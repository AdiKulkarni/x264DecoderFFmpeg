#include "rtsp.h"

rtsp::rtsp(AVFormatContext* cnt) :
        video_stream_index(0) {
    context = cnt;
}

int rtsp::init(std::string ipAddr, int port) {

    avformat_network_init();

    //open rtsp
    if (avformat_open_input(&context, "rtsp://172.28.188.118:4002", NULL, NULL)
            != 0) {
        return 1;
    }

    if (avformat_find_stream_info(context, NULL) < 0) {
        return 1;
    }

    //search video stream
    for (int i = 0; i < context->nb_streams; i++) {
        if (context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            video_stream_index = i;
    }
    av_read_play(context);    //play RTSP
}

AVFormatContext* rtsp::getRTSPContext() {
    return context;
}

int rtsp::getVideoStreamIndex() {
    return video_stream_index;
}

void rtsp::close() {
    av_read_pause(context);
}

