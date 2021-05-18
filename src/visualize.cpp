#include "utils.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

complexData data(SAMPLES);

bool MODE = true;

void clearRenderer() {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      SDL_RenderPresent(renderer);
}

bool init() {
      #ifdef OS2
      putenv("SDL_AUDIODRIVER=DirectSound");
      #endif
      if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) return false;
      
      window = SDL_CreateWindow("Musico", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
      if (window == NULL) return false;
      
      renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (renderer == NULL) return false;

      if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;

      if (TTF_Init() == -1) return false;
      
      return true;
}

int UI() {
      SDL_Surface *nwsurf = IMG_Load("assets/stscr.png");
      if (nwsurf == NULL) {
            return 0;
      }
      SDL_Texture *nwtext = NULL;
      nwtext = SDL_CreateTextureFromSurface(renderer, nwsurf);

      TTF_Font *font = TTF_OpenFont("assets/pointy.ttf", 1500);
      TTF_Font *nwfont = TTF_OpenFont("assets/rough.ttf", 1500);

      if (font == NULL) {
            return 0;
      }
      SDL_Color col = {232, 232, 232};
      const char *title = "Audio Visualizer";
      const char *op1 = "Play Music";
      const char *op2 = "Record Audio";
      const char *op3 = "Real Time Playback";
      nwsurf = TTF_RenderText_Solid(font, title, col);
      SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, nwsurf);
      nwsurf = TTF_RenderText_Solid(nwfont, op1, col);
      SDL_Texture *text1 = SDL_CreateTextureFromSurface(renderer, nwsurf);
      nwsurf = TTF_RenderText_Solid(nwfont, op2, col);
      SDL_Texture *text2 = SDL_CreateTextureFromSurface(renderer, nwsurf);
      nwsurf = TTF_RenderText_Solid(nwfont, op3, col);
      SDL_Texture *text3 = SDL_CreateTextureFromSurface(renderer, nwsurf);

      SDL_Rect rect, txrect, rect2, opt1, opt2, opt3;
      int h, w;

      int st = 0, colst = 20;
      bool quit = false;
      int val = 0;
      while (!quit) {
            SDL_Event event; 
            while (SDL_PollEvent(&event)) {
                  if (event.type == SDL_QUIT) quit = true; 

                  if (event.type == SDL_MOUSEBUTTONDOWN) {
                        int x, y; 
                        SDL_GetMouseState(&x, &y);
                        if (opt1.x <= x && x <= opt1.x + opt1.w && opt1.y <= y && y <= opt1.y + opt1.h) val = 1, quit = true;
                        else if (opt2.x <= x && x <= opt2.x + opt2.w && opt2.y <= y && y <= opt2.y + opt2.h) val = 2, quit = true;
                        else if (opt3.x <= x && x <= opt3.x + opt3.w && opt3.y <= y && y <= opt3.y + opt3.h) val = 3, quit = true;
                  }
            }
            
            SDL_GetWindowSize(window, &w, &h);
            rect.x = -1912 + st, rect.y = 0;
            rect.w = 1912, rect.h = h;
            rect2.x = st, rect2.y = 0;
            rect2.w = 1912, rect2.h = h;
            st++;
            if (rect.x == 0) st = 0;
            txrect.x = w / 7, txrect.y = h / 10;
            txrect.w = 18 * w / 25, txrect.h = h / 10;

            opt1.x = w / 3, opt1.y = h / 4 + h / 30;
            opt1.w = 10 * w / 30, opt1.h = h / 15;

            opt2.x = w / 3, opt2.y = 3 * h / 8 + h/30;
            opt2.w = w / 3, opt2.h = h / 15;

            opt3.x = w / 4, opt3.y = h / 2 + h / 30;
            opt3.w = w / 2, opt3.h = h / 15;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            SDL_RenderCopy(renderer, nwtext, NULL, &rect);
            SDL_RenderCopy(renderer, nwtext, NULL, &rect2);
            SDL_RenderCopy(renderer, text, NULL, &txrect);

            SDL_RenderCopy(renderer, text1, NULL, &opt1);
            SDL_RenderCopy(renderer, text2, NULL, &opt2);
            SDL_RenderCopy(renderer, text3, NULL, &opt3);
            SDL_RenderPresent(renderer);

            colst = (colst + 1) % 360;
      }
      SDL_DestroyTexture(text);
      SDL_DestroyTexture(nwtext);
      SDL_FreeSurface(nwsurf);
      return val;
}

double Get16bitAudioSample(Uint8 *bytebuffer, SDL_AudioFormat format) {
      Uint16 val = 0x0;

      if (format & 1 << 12) val = ((Uint16) bytebuffer[0] << 8) | (Uint16) bytebuffer[1];
      else val = (Uint16) bytebuffer[0] | ((Uint16) bytebuffer[1] << 8);

      if (format & 1 << 15) return ((int16_t)val) / (double) (1 << 15);

      return val / (double) ((1 << 16) - 1);
}

void visualizerOutput(Uint8 *stream, SDL_AudioFormat format) {
      double *MAX, *actFreq;
      MAX = (double *) malloc(sizeof(double) * BARS);
      actFreq = (double *) malloc(sizeof(double) * (BARS + 1));

      double CONSTANT = (double) SAMPLES / WINDOW_WIDTH, freq;
      static int start = 150;

      for (int i = 0; i < BARS; i++) {
            MAX[i] = 0;
            actFreq[i] = i * (RATE / SAMPLES) + i;
      }
      actFreq[BARS] = RATE / 2;
      
      for (int i = 0; i < SAMPLES; i++) {
            double multiplier = sin(pi * i / SAMPLES);
            multiplier *= multiplier;

            data.in[i][0] = Get16bitAudioSample(stream, format) * multiplier;
            data.in[i][1] = 0.0;
            stream += 2;
      }
	
      if (MODE) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            rgb col(start);
            SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, 255);
            SDL_Point *wave;
            wave = (SDL_Point*) malloc(sizeof(SDL_Point) * SAMPLES);
            for (int i = 0; i < SAMPLES; i++) {
                  wave[i].x = i / CONSTANT;
                  wave[i].y = WINDOW_HEIGHT / 2 - data.in[i][0] * VSCALE;
            }
            SDL_RenderDrawLines(renderer, wave, SAMPLES);
      }
      else {
            fftw_execute(data.plan);

            for (int j = 0; j < SAMPLES / 2; j++) {
                  double real = data.out[j][0];
                  double imaginary = data.out[j][1];

                  double magnitude = sqrt((real * real) + (imaginary * imaginary));

                  freq = j * ((double)RATE / SAMPLES);

                  for (int i = 0; i < BARS; i++) {
                        if ((freq > actFreq[i]) && (freq <= actFreq[i + 1]))
                              MAX[i] = max(MAX[i], magnitude);
                  }
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            for (int i = 0; i < BARS; i++) {
                  rgb col((start + i * 2) % 360);
                  SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, 255);

                  if (MAX[i] > 2.0) MAX[i] = log(MAX[i]);

                  for (int j = 0; j < THICKNESS; j++) {
                        SDL_RenderDrawLine(renderer, (i * DIST + j), WINDOW_HEIGHT, (i * DIST + j), WINDOW_HEIGHT - (SCALE * MAX[i]));
                  }
            }
      }
      start = (start + 1) % 360;
      SDL_RenderPresent(renderer);
      free(MAX), free(actFreq);
}

void changeMode() {
      MODE ^= 1;
}

void quit() {
      fftw_destroy_plan(data.plan);
      fftw_cleanup();
      fftw_free(data.in);
      fftw_free(data.out);
      
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      window = NULL;
      renderer = NULL;
      
      SDL_Quit();
}