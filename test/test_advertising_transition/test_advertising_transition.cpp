#include <Arduino.h>
#include <unity.h>

#include "advertising_transition.h"

void test_transition_waits_for_controller_settle_window() {
  AdvertisingTransition transition;
  transition.request(AdvertisingTarget::Seance, true, 1000);
  TEST_ASSERT_FALSE(transition.ready(1000));
  TEST_ASSERT_FALSE(transition.ready(1149));
  TEST_ASSERT_TRUE(transition.ready(1150));
}

void test_transition_is_immediately_ready_without_previous_advertisement() {
  AdvertisingTransition transition;
  transition.request(AdvertisingTarget::Normal, false, 1000);
  TEST_ASSERT_TRUE(transition.ready(1000));
}

void setup() {
  delay(500);
  UNITY_BEGIN();
  RUN_TEST(test_transition_waits_for_controller_settle_window);
  RUN_TEST(test_transition_is_immediately_ready_without_previous_advertisement);
  UNITY_END();
}

void loop() {}
