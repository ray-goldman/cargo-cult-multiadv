#pragma once

#include <stdint.h>

#include "role_profiles.h"

constexpr uint8_t normalInstanceMask(Role role) {
  return role == Role::Acolyte ? 0b0111 : 0b0001;
}
