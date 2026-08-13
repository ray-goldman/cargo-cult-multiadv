#pragma once

#include <stdint.h>

enum class AdvertisingTarget : uint8_t { Normal, Seance };

class AdvertisingTransition {
 public:
  void request(AdvertisingTarget target, bool needsSettlement, uint32_t nowMs);
  bool ready(uint32_t nowMs) const;
  bool pending() const;
  AdvertisingTarget target() const;
  void reset();

 private:
  AdvertisingTarget target_ = AdvertisingTarget::Normal;
  uint32_t readyAtMs_ = 0;
  bool pending_ = false;
};
