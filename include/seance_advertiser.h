#pragma once

#include <Arduino.h>

class SeanceAdvertiser {
 public:
  bool begin();
  bool start();
  void stop();
  bool active() const;
  const char* lastError() const;

 private:
  bool active_ = false;
  const char* lastError_ = "not started";
};
