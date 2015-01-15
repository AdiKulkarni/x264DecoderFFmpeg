#include "decoder.h"

decoder::decoder() :
        codec(NULL), codecContext(NULL), frame_count(0), mPlayer(1280, 720), frame(
                NULL), parser(NULL), recordOldTime(0) {
}

void decoder::initialize() {

    /* register all the codecs */
    avcodec_register_all();
    av_init_packet(&avpkt);

    /* set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams) */
    memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

    /* find the x264 video decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    /* put sample parameters */
    //codecContext->bit_rate = 500000;
    //codecContext->width = 640;
    //codecContext->height = 480;
    //codecContext->time_base = (AVRational ) { 1, 15 };
    //codecContext->framerate = (AVRational ) { 1, 15 };
    //codecContext->gop_size = 1;
    //codecContext->max_b_frames = 0;
    //codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    //codecContext->ticks_per_frame = 0;
    //codecContext->delay = 0;
    //codecContext->b_quant_offset = 0.0;
    //codecContext->refs = 0;
    //codecContext->slices = 1;
    //codecContext->has_b_frames = 0;
    //codecContext->thread_count = 2;
    //av_opt_set(codecContext->priv_data, "zerolatency", "ultrafast", 0);

    /* we do not send complete frames */
    if (codec->capabilities & CODEC_CAP_TRUNCATED)
        codecContext->flags |= CODEC_FLAG_TRUNCATED;

    /* open it */
    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    parser = av_parser_init(AV_CODEC_ID_H264);
    if (!parser) {
        std::cout << "cannot create parser" << std::endl;
        exit(1);
    }
    parser->flags |= PARSER_FLAG_ONCE;

    frame_count = 0;
    mPlayer.initialize();
}

int decoder::decodeFrame(const char *data, int length) {

    // Parse input stream to check if there is a valid frame.
    int parsedLength = av_parser_parse2(parser, codecContext, &avpkt.data,
            &avpkt.size, (const uint8_t*) data, length, AV_NOPTS_VALUE,
            AV_NOPTS_VALUE, -1);

    if (!avpkt.size) {

        // Not enough data
        // std::cout <<frame_count<< " : Not enough data" << std::endl;
        return parsedLength;

    } else {

        while (avpkt.size > 0) {

            int len, got_frame;
            len = avcodec_decode_video2(codecContext, frame, &got_frame,
                    &avpkt);

            if (len < 0) {
                fprintf(stderr, "Error while decoding frame %d\n", frame_count);
                exit(1);
            }

            if (got_frame) {

                frame_count++;

                pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
                long milliseconds = current_date_microseconds.time_of_day().total_milliseconds();
                pt::time_duration current_time_milliseconds = pt::milliseconds(milliseconds);
                pt::ptime current_date_milliseconds(current_date_microseconds.date(), current_time_milliseconds);

                std::cout << frame_count<<" "<< (milliseconds - recordOldTime) << std::endl;
                recordOldTime = milliseconds;

                mPlayer.showFrameOnDisplay(frame);

            }

            if (avpkt.data) {
                avpkt.size -= len;
                avpkt.data += len;
            }
        }
        return length;
    }
}

void decoder::close() {

    avpkt.data = NULL;
    avpkt.size = 0;
    if (parser) {
        av_parser_close(parser);
        parser = NULL;
    }
    avcodec_close(codecContext);
    av_free(codecContext);
    av_frame_free(&frame);
    printf("\n");
}
