#pragma once

#include <stdint.h>

#include "role_profiles.h"

class RoleController {
 public:
  explicit RoleController(uint32_t intervalMs);
  Role role() const;
  bool automatic() const;
  bool seancePhase() const;
  uint32_t remainingMs(uint32_t now) const;
  bool tick(uint32_t now);
  void setAutomatic(uint32_t now);
  void setManual(Role role, uint32_t now);
  void next(uint32_t now);

 private:
  Role role_;
  uint32_t interval_;
  uint32_t last_;
  bool auto_;
  bool seance_;
};
