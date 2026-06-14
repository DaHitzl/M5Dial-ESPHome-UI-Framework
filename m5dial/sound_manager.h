#pragma once
#include "esphome.h"

enum SoundType {
  SOUND_TOUCH,
  SOUND_BACK,
  SOUND_LEFT,
  SOUND_RIGHT,
  SOUND_BUTTON
};

class SoundManager {
public:
  static void play(SoundType type, bool tone_enabled) {
    if (!tone_enabled) return;
    switch(type) {
      case SOUND_TOUCH:  id(play_touch).execute(); break;
      case SOUND_BACK:   id(play_back).execute(); break;
      case SOUND_LEFT:   id(play_click_left).execute(); break;
      case SOUND_RIGHT:  id(play_click_right).execute(); break;
      case SOUND_BUTTON: id(play_button_deep).execute(); break;
    }
  }
};