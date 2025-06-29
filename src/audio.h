#ifndef AUDIO_H_
#define AUDIO_H_
#include <raylib.h>
enum AudioName {
    AUDIO_SLICE_1 = 0,
    AUDIO_SLICE_2 = 1,
    AUDIO_SLICE_3 = 2,
    AUDIO_SLICE_4 = 3,
    AUDIO_BOOM_1 = 4,
    AUDIO_BOOM_2 = 5,
    _N_AUDIO
};

enum MusicName { MUSIC_MENU = 0, _N_MUSIC };

Sound get_sound(enum AudioName track);

Music get_music(enum MusicName track);
#endif // AUDIO_H_
