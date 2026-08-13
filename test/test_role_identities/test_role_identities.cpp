#include <Arduino.h>
#include <string.h>
#include <unity.h>

#include "role_identities.h"
#include "role_profiles.h"

namespace {
constexpr Role kRoles[] = {Role::Acolyte, Role::Glyph, Role::Elder, Role::Cthulhu};
constexpr size_t kFamilyAnchorIndexes[] = {0, 8, 16, 24};
constexpr const char* kExpectedFamilyAnchors[4][4] = {
    {"N0X_001", "H3XDUMP", "SYSC4LL", "R0PCH41"},
    {"GLYPH01", "S1G1L01", "W4RD_01", "S3AL_01"},
    {"ELDER01", "H4STUR1", "Y0GS0T1", "D4G0N01"},
    {"CTHUL01", "RLY3H01", "T3NT4C1", "4WAK3N1"},
};
}

void test_every_identity_address_is_static_random() {
  for (Role role : kRoles) {
    for (size_t index = 0; index < roleIdentityCount(role); ++index) {
      const RoleIdentity identity = roleIdentity(role, index);
      TEST_ASSERT_EQUAL_UINT8(0xc0, identity.address[0] & 0xc0);
    }
  }
}

void test_all_identities_are_mutually_distinct() {
  // A target badge stores one member per unique peer, so a repeated address
  // anywhere in the pool would silently cost a recruit.
  for (size_t a = 0; a < 4; ++a) {
    for (size_t i = 0; i < roleIdentityCount(kRoles[a]); ++i) {
      const RoleIdentity left = roleIdentity(kRoles[a], i);
      for (size_t b = 0; b < 4; ++b) {
        for (size_t j = 0; j < roleIdentityCount(kRoles[b]); ++j) {
          if (a == b && i == j) continue;
          const RoleIdentity right = roleIdentity(kRoles[b], j);
          TEST_ASSERT_NOT_EQUAL(0, memcmp(left.address, right.address, kRoleIdentityAddressLength));
          TEST_ASSERT_NOT_EQUAL(0, strcmp(left.name, right.name));
        }
      }
    }
  }
}

void test_identity_names_survive_the_badge_seven_character_limit() {
  // A real badge dump stored every peer name truncated to seven characters, so
  // names must stay distinguishable within that width.
  for (Role role : kRoles) {
    for (size_t index = 0; index < roleIdentityCount(role); ++index) {
      const size_t length = strlen(roleIdentity(role, index).name);
      TEST_ASSERT_TRUE(length > 0 && length <= 7);
    }
  }

  for (size_t a = 0; a < 4; ++a) {
    for (size_t i = 0; i < roleIdentityCount(kRoles[a]); ++i) {
      for (size_t b = 0; b < 4; ++b) {
        for (size_t j = 0; j < roleIdentityCount(kRoles[b]); ++j) {
          if (a == b && i == j) continue;
          TEST_ASSERT_NOT_EQUAL(0, strncmp(roleIdentity(kRoles[a], i).name,
                                           roleIdentity(kRoles[b], j).name, 7));
        }
      }
    }
  }
}

void test_role_identity_pools_use_expected_role_families() {
  for (size_t roleIndex = 0; roleIndex < 4; ++roleIndex) {
    TEST_ASSERT_EQUAL_UINT32(32, roleIdentityCount(kRoles[roleIndex]));
    for (size_t anchor = 0; anchor < 4; ++anchor) {
      TEST_ASSERT_EQUAL_STRING(kExpectedFamilyAnchors[roleIndex][anchor],
                               roleIdentity(kRoles[roleIndex], kFamilyAnchorIndexes[anchor]).name);
    }
  }
}

void test_rotation_advances_per_role_and_wraps() {
  IdentityRotation rotation;
  const RoleIdentity first = rotation.current(Role::Glyph);
  rotation.advance(Role::Glyph);
  const RoleIdentity second = rotation.current(Role::Glyph);
  TEST_ASSERT_NOT_EQUAL(0, memcmp(first.address, second.address, kRoleIdentityAddressLength));

  // Advancing one role must not disturb another.
  TEST_ASSERT_EQUAL_UINT32(0, rotation.index(Role::Elder));

  for (size_t step = 1; step < roleIdentityCount(Role::Glyph); ++step) {
    rotation.advance(Role::Glyph);
  }
  TEST_ASSERT_EQUAL_UINT32(0, rotation.index(Role::Glyph));
  const RoleIdentity wrapped = rotation.current(Role::Glyph);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(first.address, wrapped.address, kRoleIdentityAddressLength);
}

void test_rotated_identity_builds_a_valid_manufacturer_packet() {
  IdentityRotation rotation;
  const RoleIdentity identity = rotation.current(Role::Elder);
  uint8_t data[20] = {};
  const size_t length = buildManufacturerData(Role::Elder, identity.name, data, sizeof(data));
  TEST_ASSERT_EQUAL_UINT32(7 + strlen(identity.name), length);
  TEST_ASSERT_EQUAL_UINT8(0x0d, data[0]);
  TEST_ASSERT_EQUAL_UINT8(0xf0, data[1]);
  TEST_ASSERT_EQUAL_UINT8('E', data[2]);
  TEST_ASSERT_EQUAL_UINT8('L', data[3]);
  TEST_ASSERT_EQUAL_UINT8('D', data[4]);
  TEST_ASSERT_EQUAL_UINT8('R', data[5]);
  TEST_ASSERT_EQUAL_UINT8(0x03, data[6]);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(identity.name, data + 7, strlen(identity.name));
}

void setup() {
  delay(500);
  UNITY_BEGIN();
  RUN_TEST(test_every_identity_address_is_static_random);
  RUN_TEST(test_all_identities_are_mutually_distinct);
  RUN_TEST(test_identity_names_survive_the_badge_seven_character_limit);
  RUN_TEST(test_role_identity_pools_use_expected_role_families);
  RUN_TEST(test_rotation_advances_per_role_and_wraps);
  RUN_TEST(test_rotated_identity_builds_a_valid_manufacturer_packet);
  UNITY_END();
}

void loop() {}
