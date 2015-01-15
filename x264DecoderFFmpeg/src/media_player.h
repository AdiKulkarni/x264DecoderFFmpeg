//framePlayer

#ifndef X264_EXAMPLE_MEDIA_PLAYER_H
#define X264_EXAMPLE_MEDIA_PLAYER_H

extern "C"{

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/audioconvert.h>
#include <libavutil/common.h>
#include <libavformat/avio.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>

}

class media_player{

    private:

         int             d_height;
         int             d_width;
         SDL_Overlay     *bmp;
         SDL_Surface     *screen;
         SDL_Rect        rect;
         SDL_Event       event;
         struct SwsContext* sws;
         AVFrame         *pFrame;

    public:

         media_player(int width, int height);
         void initialize();
         void showFrameOnDisplay(AVFrame *pFrame);
         void close();
};

#endif
