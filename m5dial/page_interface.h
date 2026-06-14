#pragma once
#include "esphome.h"
#include <functional>

class PageInterface {
public:
  virtual void draw(esphome::display::Display &it) = 0;
  virtual void on_encoder(int step) {}
  virtual void on_button() {}
  virtual void on_touch(int x, int y) {}
  virtual ~PageInterface() {}
};