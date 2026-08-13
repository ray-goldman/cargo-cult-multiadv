#include "role_profiles.h"

#include <string.h>

namespace {
constexpr RoleProfile kProfiles[] = {
    {Role::Acolyte, "Acolyte", 0x01},
    {Role::Glyph, "Glyph", 0x02},
    {Role::Elder, "Elder", 0x03},
    {Role::Cthulhu, "Cthulhu", 0x04},
};

constexpr size_t kHeaderLength = 7;
constexpr size_t kMaximumNameLength = 10;

// Member-exchange tags, one per role. The firmware's dispatcher compares
// payload bytes 2-5 against these before it will record a peer.
constexpr char kCultTag[] = "CULT";
constexpr char kGlyphTag[] = "GLPB";
constexpr char kElderTag[] = "ELDR";
constexpr char kCthulhuTag[] = "CTHU";
}  // namespace

const char* tagFor(Role role) {
  switch (role) {
    case Role::Glyph: return kGlyphTag;
    case Role::Elder: return kElderTag;
    case Role::Cthulhu: return kCthulhuTag;
    case Role::Acolyte:
    default: return kCultTag;
  }
}

const RoleProfile& profileFor(Role role) {
  for (const RoleProfile& profile : kProfiles) {
    if (profile.role == role) return profile;
  }
  return kProfiles[0];
}

Role nextRole(Role role) {
  switch (role) {
    case Role::Acolyte: return Role::Glyph;
    case Role::Glyph: return Role::Elder;
    case Role::Elder: return Role::Cthulhu;
    case Role::Cthulhu:
    default: return Role::Acolyte;
  }
}

size_t buildManufacturerData(Role role, const char* name, uint8_t* destination, size_t capacity,
                             const char* tag) {
  if (destination == nullptr || capacity < kHeaderLength || name == nullptr) return 0;
  if (tag == nullptr) tag = tagFor(role);

  // Bytes 0-1: company identifier 0xF00D, little-endian on air.
  destination[0] = 0x0d;
  destination[1] = 0xf0;
  // Bytes 2-5: protocol tag.
  memcpy(destination + 2, tag, kTagLength);
  // Byte 6: role. The parser rejects a zero here, and every role code is 1-4.
  destination[6] = profileFor(role).protocolType;

  const size_t nameLength = strnlen(name, kMaximumNameLength);
  if (capacity < kHeaderLength + nameLength) return 0;
  memcpy(destination + kHeaderLength, name, nameLength);
  return kHeaderLength + nameLength;
}
