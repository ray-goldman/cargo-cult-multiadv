#include "unity_config.h"

void unityOutputStart(void) {
  Serial.begin(115200);
}

void unityOutputChar(unsigned int c) {
  Serial.write(static_cast<uint8_t>(c));
}

void unityOutputFlush(void) {
  Serial.flush();
}

void unityOutputComplete(void) {}
