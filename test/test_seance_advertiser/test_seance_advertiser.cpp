#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEExtAdvertising.h>
#include <unity.h>

#include "seance_advertiser.h"

void test_seance_advertiser_starts_inactive() {
  SeanceAdvertiser advertiser;
  TEST_ASSERT_FALSE(advertiser.active());
}

void test_seance_starts_after_the_controller_stops_normal_advertising() {
  NimBLEDevice::init("");
  NimBLEExtAdvertising* advertising = NimBLEDevice::getAdvertising();
  NimBLEExtAdvertisement normal;
  normal.setLegacyAdvertising(true);
  normal.setConnectable(false);
  normal.setScannable(false);
  normal.setFlags(0x06);
  TEST_ASSERT_TRUE(advertising->setInstanceData(0, normal));
  TEST_ASSERT_TRUE(advertising->start(0));
  // queueAdvertising() owns the physical handoff between advertising modes.
  TEST_ASSERT_TRUE(advertising->stop(0));

  SeanceAdvertiser seance;
  TEST_ASSERT_TRUE(seance.start());
  TEST_ASSERT_TRUE(seance.active());
  seance.stop();
}

void setup() {
  delay(500);
  UNITY_BEGIN();
  RUN_TEST(test_seance_advertiser_starts_inactive);
  RUN_TEST(test_seance_starts_after_the_controller_stops_normal_advertising);
  UNITY_END();
}

void loop() {}
