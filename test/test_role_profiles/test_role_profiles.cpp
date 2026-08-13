#include <Arduino.h>
#include <unity.h>

#include "role_profiles.h"

void test_badge_role_codes_match_the_observed_protocol() {
  TEST_ASSERT_EQUAL_UINT8(1, profileFor(Role::Acolyte).protocolType);
  TEST_ASSERT_EQUAL_UINT8(2, profileFor(Role::Glyph).protocolType);
  TEST_ASSERT_EQUAL_UINT8(3, profileFor(Role::Elder).protocolType);
  TEST_ASSERT_EQUAL_UINT8(4, profileFor(Role::Cthulhu).protocolType);
}

void test_seance_packet_matches_the_observed_acolyte_capture() {
  // The docs/ble-capture.md payload was recorded while the badge was in Seance
  // mode, so it carries the SEAN tag.
  uint8_t data[20] = {};
  const size_t length =
      buildManufacturerData(Role::Acolyte, "RAYGOLDMAN", data, sizeof(data), kSeanceTag);
  const uint8_t expected[] = {0x0d, 0xf0, 'S', 'E', 'A', 'N', 0x01,
                              'R', 'A', 'Y', 'G', 'O', 'L', 'D', 'M', 'A', 'N'};
  TEST_ASSERT_EQUAL_UINT32(sizeof(expected), length);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, data, sizeof(expected));
}

void test_member_exchange_packets_use_the_cult_family_tags() {
  // The firmware's member parser dispatches on these tags; SEAN reaches only
  // the Seance path, which is why single roles never registered.
  TEST_ASSERT_EQUAL_STRING("CULT", tagFor(Role::Acolyte));
  TEST_ASSERT_EQUAL_STRING("GLPB", tagFor(Role::Glyph));
  TEST_ASSERT_EQUAL_STRING("ELDR", tagFor(Role::Elder));
  TEST_ASSERT_EQUAL_STRING("CTHU", tagFor(Role::Cthulhu));

  uint8_t data[20] = {};
  const size_t length = buildManufacturerData(Role::Acolyte, "ACOLYT1", data, sizeof(data));
  const uint8_t expected[] = {0x0d, 0xf0, 'C', 'U', 'L', 'T', 0x01,
                              'A', 'C', 'O', 'L', 'Y', 'T', '1'};
  TEST_ASSERT_EQUAL_UINT32(sizeof(expected), length);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, data, sizeof(expected));
  // The parser requires at least seven bytes and a non-zero role byte.
  TEST_ASSERT_TRUE(length >= 7);
  TEST_ASSERT_NOT_EQUAL(0, data[6]);
}

void setup() {
  delay(500);
  UNITY_BEGIN();
  RUN_TEST(test_badge_role_codes_match_the_observed_protocol);
  RUN_TEST(test_seance_packet_matches_the_observed_acolyte_capture);
  RUN_TEST(test_member_exchange_packets_use_the_cult_family_tags);
  UNITY_END();
}

void loop() {}
