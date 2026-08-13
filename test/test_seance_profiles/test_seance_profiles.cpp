#include <Arduino.h>
#include <string.h>
#include <unity.h>

#include "role_profiles.h"
#include "seance_profiles.h"

void test_seance_has_three_distinct_non_acolyte_peers() {
  resetSeancePeers();
  TEST_ASSERT_EQUAL_UINT(3, seancePeerCount());
  const SeancePeer* peers = seancePeers();
  TEST_ASSERT_EQUAL_UINT8(2, profileFor(peers[0].role).protocolType);
  TEST_ASSERT_EQUAL_UINT8(3, profileFor(peers[1].role).protocolType);
  TEST_ASSERT_EQUAL_UINT8(4, profileFor(peers[2].role).protocolType);
  TEST_ASSERT_NOT_EQUAL(0, memcmp(peers[0].address, peers[1].address, 6));
  TEST_ASSERT_NOT_EQUAL(0, memcmp(peers[1].address, peers[2].address, 6));
  TEST_ASSERT_EQUAL_UINT8(0xc0, peers[0].address[0] & 0xc0);
  TEST_ASSERT_EQUAL_UINT8(0xc0, peers[1].address[0] & 0xc0);
  TEST_ASSERT_EQUAL_UINT8(0xc0, peers[2].address[0] & 0xc0);
}

void test_first_peer_set_matches_the_captured_addresses() {
  // Set 0 must stay byte-identical to docs/ble-capture.md so the first Seance
  // after a reset reproduces the verified behaviour.
  resetSeancePeers();
  const SeancePeer* peers = seancePeers();
  const uint8_t glyph[] = {0xc2, 0x54, 0x53, 0x45, 0x41, 0x02};
  const uint8_t elder[] = {0xc2, 0x54, 0x53, 0x45, 0x41, 0x03};
  const uint8_t cthulhu[] = {0xc2, 0x54, 0x53, 0x45, 0x41, 0x04};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(glyph, peers[0].address, 6);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(elder, peers[1].address, 6);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(cthulhu, peers[2].address, 6);
  TEST_ASSERT_EQUAL_STRING("SEANGLYPH1", peers[0].name);
  TEST_ASSERT_EQUAL_STRING("SEANELDER1", peers[1].name);
  TEST_ASSERT_EQUAL_STRING("SEANCTHUL1", peers[2].name);
}

void test_advancing_presents_new_peer_addresses_then_wraps() {
  resetSeancePeers();
  uint8_t firstGlyph[6] = {};
  memcpy(firstGlyph, seancePeers()[0].address, 6);

  advanceSeancePeers();
  TEST_ASSERT_EQUAL_UINT32(1, seanceSetIndex());
  TEST_ASSERT_NOT_EQUAL(0, memcmp(firstGlyph, seancePeers()[0].address, 6));

  for (size_t step = 1; step < seanceSetCount(); ++step) {
    advanceSeancePeers();
  }
  TEST_ASSERT_EQUAL_UINT32(0, seanceSetIndex());
  TEST_ASSERT_EQUAL_UINT8_ARRAY(firstGlyph, seancePeers()[0].address, 6);
}

void test_later_seance_sets_use_short_unique_hacker_aliases() {
  const char* expected[][3] = {
      {"DEADC0D", "0FFGRID", "WH1SPR_"},
      {"N3TB0T_", "SP00FER", "3XPL0IT"},
      {"C0BALT_", "V1RUS_X", "C4CH3D_"},
  };
  resetSeancePeers();
  for (size_t set = 1; set < seanceSetCount(); ++set) {
    advanceSeancePeers();
    const SeancePeer* peers = seancePeers();
    for (size_t peer = 0; peer < seancePeerCount(); ++peer) {
      TEST_ASSERT_EQUAL_STRING(expected[set - 1][peer], peers[peer].name);
      TEST_ASSERT_TRUE(strlen(peers[peer].name) <= 7);
    }
  }
}

void setup() {
  delay(500);
  UNITY_BEGIN();
  RUN_TEST(test_seance_has_three_distinct_non_acolyte_peers);
  RUN_TEST(test_first_peer_set_matches_the_captured_addresses);
  RUN_TEST(test_advancing_presents_new_peer_addresses_then_wraps);
  RUN_TEST(test_later_seance_sets_use_short_unique_hacker_aliases);
  UNITY_END();
}

void loop() {}
