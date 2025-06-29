#include "audio.h"
#include <raylib.h>

static const char *audio[] = {
    [AUDIO_SLICE_1] = "resource/goodart/slice_1.wav",
    [AUDIO_SLICE_2] = "resource/goodart/slice_2.wav",
    [AUDIO_SLICE_3] = "resource/goodart/slice_3.wav",
    [AUDIO_SLICE_4] = "resource/goodart/slice_4.wav",
    [AUDIO_BOOM_1] = "resource/goodart/boom.wav",
    [AUDIO_BOOM_2] = "resource/goodart/boom_shut_up_dan_variant.wav"};
static const char *music[] = {
    [MUSIC_MENU] = "resource/goodart/menu.wav",
};
static Sound loaded_audio[_N_AUDIO] = {0};
static Music loaded_music[_N_MUSIC] = {0};

static void load_sound(enum AudioName track) {
  loaded_audio[track] = LoadSound(audio[track]);
}

static void load_music(enum MusicName track) {
  loaded_music[track] = LoadMusicStream(music[track]);
}

Sound get_sound(enum AudioName track) {
  if (loaded_audio[track].frameCount == 0) {
    load_sound(track);
  }
  return loaded_audio[track];
}

Music get_music(enum MusicName track) {
  if (loaded_music[track].frameCount == 0) {
    load_music(track);
  }
  return loaded_music[track];
}
