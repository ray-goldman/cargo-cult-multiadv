#pragma once

#include <stddef.h>

#include "role_identities.h"

constexpr size_t kAcolytePeerCount = 3;

class AcolyteAdvertiser {
 public:
  bool start(IdentityRotation& rotation);
  void stop();
  bool active() const;
  const char* name(size_t index) const;
  const char* lastError() const;

 private:
  bool active_ = false;
  const char* names_[kAcolytePeerCount] = {"", "", ""};
  const char* lastError_ = "not started";
};
