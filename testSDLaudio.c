// compile with gcc testSDLaudio.c -I /usr/include/SDL -lpthread -lSDL -lSDL_mixer -o testSDLaudio

#include <SDL.h>
#include "SDL_mixer.h"

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

sem_t soundfinished;
struct timeval time1, time2, time3, time4, time5;

void sound_ended(int x) {
    sem_post(&soundfinished);
}

long long timeval_diff(struct timeval *end_time, struct timeval *start_time) {
    struct timeval diff;
    struct timeval *difference = &diff;

    difference->tv_sec =end_time->tv_sec -start_time->tv_sec ;
    difference->tv_usec=end_time->tv_usec-start_time->tv_usec;

    while(difference->tv_usec<0) {
        difference->tv_usec += 1000000;
        difference->tv_sec  -= 1;
    }

    return 1000000LL*difference->tv_sec+
                     difference->tv_usec;
}

int main(int argc, char** argv)
{
    Mix_Chunk *wav;
    sem_init(&soundfinished, 0, 0);

    gettimeofday(&time1, NULL);

    // Inilialize SDL_mixer , exit if fail
    if (SDL_Init(SDL_INIT_AUDIO) < 0) exit(1);
    // Setup audio mode
    Mix_OpenAudio(22050,AUDIO_S16SYS,2,640);

    gettimeofday(&time2, NULL);

    wav = Mix_LoadWAV(argv[1]);
    if (!wav) {
         fprintf(stderr,"Could not load file, pass filename as parameter.\n");
         exit(1);
    }

    gettimeofday(&time3, NULL);

    Mix_PlayChannel(-1, wav, 0);
    Mix_ChannelFinished(&sound_ended);

    sem_wait(&soundfinished);

    gettimeofday(&time4, NULL);

    Mix_FreeChunk(wav);
    SDL_CloseAudio();
    SDL_Quit();

    gettimeofday(&time5, NULL);

    sem_destroy(&soundfinished);
    printf("init SDL AUDIO takes     %16lld microseconds\n", timeval_diff(&time2, &time1));
    printf("loading audio file takes %16lld microseconds\n", timeval_diff(&time3, &time2));
    printf("playing AUDIO takes      %16lld microseconds\n", timeval_diff(&time4, &time3));
    printf("de-init SDL AUDIO takes  %16lld microseconds\n", timeval_diff(&time5, &time4));
}
