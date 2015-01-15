#include "media_player.h"

media_player::media_player(int width, int height) :
        bmp(NULL), screen(NULL), pFrame(NULL), sws(NULL) {

    d_width = width;
    d_height = height;

}

void media_player::initialize() {

    sws = sws_getContext(d_width, d_height, PIX_FMT_YUV420P, d_width, d_height,
            PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);

    if (!sws) {
        fprintf(stderr, "Cannot create SWS context");
        exit(1);
    }

    pFrame = av_frame_alloc();
    if (!pFrame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        exit(1);
    }

    // Make a screen to put our video
    #ifndef __DARWIN__
        screen = SDL_SetVideoMode(d_width, d_height, 0, 0);
    #else
        screen = SDL_SetVideoMode(d_width, d_height, 24, 0);
    #endif

    if (!screen) {
        fprintf(stderr, "SDL: could not set video mode - exiting\n");
        exit(1);
    }

    // Allocate a place to put our YUV image on that screen
    bmp = SDL_CreateYUVOverlay(d_width, d_height, SDL_YV12_OVERLAY, screen);
}

void media_player::showFrameOnDisplay(AVFrame *frame) {

    SDL_LockYUVOverlay(bmp);

    pFrame->data[0] = bmp->pixels[0];
    pFrame->data[1] = bmp->pixels[2];
    pFrame->data[2] = bmp->pixels[1];
    pFrame->linesize[0] = bmp->pitches[0];
    pFrame->linesize[1] = bmp->pitches[2];
    pFrame->linesize[2] = bmp->pitches[1];

    if (d_height
            != sws_scale(sws, frame->data, frame->linesize, 0, d_height,
                    pFrame->data, pFrame->linesize)) {
        fprintf(stderr, "scale failed");
        exit(1);
    }

    SDL_UnlockYUVOverlay(bmp);

    rect.x = 0;
    rect.y = 0;
    rect.w = d_width;
    rect.h = d_height;
    SDL_DisplayYUVOverlay(bmp, &rect);

}

void media_player::close() {

    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        SDL_Quit();
            exit(0);
        break;

    default:
        break;
    }

    if (sws) {
        sws_freeContext(sws);
        sws = NULL;
    }
    av_frame_free(&pFrame);
}
