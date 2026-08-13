#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEExtAdvertising.h>
#include <string.h>
#include <unity.h>

#include "acolyte_advertiser.h"
#include "normal_phase.h"
#include "role_identities.h"

namespace {

void stopAcolyteInstances() {
  NimBLEExtAdvertising* advertising = NimBLEDevice::getAdvertising();
  for (uint8_t instance = 0; instance < kAcolytePeerCount; ++instance) {
    advertising->stop(instance);
  }
}

}  // namespace

void test_acolyte_burst_starts_three_distinct_identities() {
  NimBLEDevice::init("");
  IdentityRotation rotation;
  AcolyteAdvertiser advertiser;

  TEST_ASSERT_TRUE(advertiser.start(rotation));
  TEST_ASSERT_TRUE(advertiser.active());
  TEST_ASSERT_EQUAL_UINT32(3, rotation.index(Role::Acolyte));
  TEST_ASSERT_NOT_EQUAL(0, strcmp(advertiser.name(0), advertiser.name(1)));
  TEST_ASSERT_NOT_EQUAL(0, strcmp(advertiser.name(1), advertiser.name(2)));
  advertiser.stop();
  stopAcolyteInstances();
}

void test_acolyte_burst_rotation_wraps_after_three_starts() {
  NimBLEDevice::init("");
  IdentityRotation rotation;
  // Park two short of the end so a three-identity burst wraps past zero,
  // whatever the pool size is.
  const size_t poolSize = roleIdentityCount(Role::Acolyte);
  for (size_t step = 0; step < poolSize - 2; ++step) rotation.advance(Role::Acolyte);

  AcolyteAdvertiser advertiser;
  TEST_ASSERT_TRUE(advertiser.start(rotation));
  TEST_ASSERT_EQUAL_UINT32(1, rotation.index(Role::Acolyte));
  advertiser.stop();
  stopAcolyteInstances();
}

void test_only_acolyte_uses_all_three_advertising_instances() {
  TEST_ASSERT_EQUAL_UINT8(0b0111, normalInstanceMask(Role::Acolyte));
  TEST_ASSERT_EQUAL_UINT8(0b0001, normalInstanceMask(Role::Glyph));
  TEST_ASSERT_EQUAL_UINT8(0b0001, normalInstanceMask(Role::Elder));
  TEST_ASSERT_EQUAL_UINT8(0b0001, normalInstanceMask(Role::Cthulhu));
}

void setup() {
  delay(500);
  UNITY_BEGIN();
  RUN_TEST(test_acolyte_burst_starts_three_distinct_identities);
  RUN_TEST(test_acolyte_burst_rotation_wraps_after_three_starts);
  RUN_TEST(test_only_acolyte_uses_all_three_advertising_instances);
  UNITY_END();
}

void loop() {}
