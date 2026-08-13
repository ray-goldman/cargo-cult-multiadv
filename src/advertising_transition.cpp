#include "advertising_transition.h"

namespace {
constexpr uint32_t kControllerSettleMs = 150;
}

void AdvertisingTransition::request(AdvertisingTarget target, bool needsSettlement, uint32_t nowMs) {
  target_ = target;
  readyAtMs_ = nowMs + (needsSettlement ? kControllerSettleMs : 0);
  pending_ = true;
}

bool AdvertisingTransition::ready(uint32_t nowMs) const {
  return pending_ && static_cast<int32_t>(nowMs - readyAtMs_) >= 0;
}
bool AdvertisingTransition::pending() const { return pending_; }
AdvertisingTarget AdvertisingTransition::target() const { return target_; }
void AdvertisingTransition::reset() {
  readyAtMs_ = 0;
  pending_ = false;
}
