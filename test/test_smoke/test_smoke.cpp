#include <Arduino.h>
#include <unity.h>

void test_project_builds() {
  TEST_ASSERT_TRUE(true);
}

void setup() {
  delay(500);
  UNITY_BEGIN();
  RUN_TEST(test_project_builds);
  UNITY_END();
}

void loop() {}
