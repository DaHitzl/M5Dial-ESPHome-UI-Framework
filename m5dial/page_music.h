#pragma once
#include "page_interface.h"
#include "ui_helpers.h"

class MusicPage : public PageInterface {
public:
  void draw(esphome::display::Display &it) override {
    it.image(0, 0, &id(img_music));
    UIHelpers::draw_header(it, "Music Player", Color(0, 255, 0));
    it.print(120, 100, &id(meine_schrift), Color(255, 255, 255), TextAlign::CENTER, "WIP / V0.7.0");
  }
};