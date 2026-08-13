#pragma once

#include <stddef.h>
#include <stdint.h>

#include "role_profiles.h"

// Length of a BLE device address, in bytes.
constexpr size_t kRoleIdentityAddressLength = 6;

// Number of distinct identities available per role.
constexpr size_t kIdentitiesPerRole = 32;

// A single advertising identity. A target badge records one member entry per
// unique peer identity, so the address is what makes an advertisement count as
// a new recruit rather than a repeat sighting of a known member.
struct RoleIdentity {
  const uint8_t* address;  // kRoleIdentityAddressLength bytes, static-random
  const char* name;        // at most ten characters
};

size_t roleIdentityCount(Role role);
RoleIdentity roleIdentity(Role role, size_t index);

// Tracks which identity each role should advertise next. Advancing after every
// successful advertisement is what lets a single emulator register more than
// one member per role on the same target badge.
class IdentityRotation {
 public:
  RoleIdentity current(Role role) const;
  size_t index(Role role) const;
  void advance(Role role);
  void setIndex(Role role, size_t index);
  void reset();

 private:
  uint8_t index_[4] = {0, 0, 0, 0};
};
