#pragma once

#include <stddef.h>
#include <stdint.h>

enum class Role : uint8_t {
  Acolyte = 1,
  Glyph = 2,
  Elder = 3,
  Cthulhu = 4,
};

struct RoleProfile {
  Role role;
  const char* displayName;
  uint8_t protocolType;
};

// Four-character protocol tag carried at payload offset 2, immediately after the
// 0xF00D company identifier. The firmware dispatches on this tag: the ordinary
// member-exchange parser accepts CULT, GLPR, GLPG, GLPB, ELDR and CTHU, while
// SEAN is handled only by the separate Seance path.
constexpr size_t kTagLength = 4;
constexpr char kSeanceTag[] = "SEAN";

// The member-exchange tag matching a role.
const char* tagFor(Role role);

const RoleProfile& profileFor(Role role);
Role nextRole(Role role);

// Builds the manufacturer payload. Passing nullptr for tag uses tagFor(role);
// the Seance advertiser passes kSeanceTag explicitly.
size_t buildManufacturerData(Role role, const char* name, uint8_t* destination, size_t capacity,
                             const char* tag = nullptr);
