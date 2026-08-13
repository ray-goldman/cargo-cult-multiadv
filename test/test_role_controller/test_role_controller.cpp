#include <Arduino.h>
#include <unity.h>

#include "role_controller.h"

void test_automatic_mode_changes_role_at_60000_ms() {
  RoleController controller(60000);
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(Role::Acolyte), static_cast<uint8_t>(controller.role()));
  TEST_ASSERT_FALSE(controller.tick(59999));
  TEST_ASSERT_TRUE(controller.tick(60000));
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(Role::Glyph), static_cast<uint8_t>(controller.role()));
}

void test_manual_role_does_not_change_after_an_interval() {
  RoleController controller(60000);
  controller.setManual(Role::Elder, 0);
  TEST_ASSERT_FALSE(controller.tick(120000));
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(Role::Elder), static_cast<uint8_t>(controller.role()));
}

void test_automatic_mode_enters_seance_after_cthulhu_then_restarts_acolyte() {
  RoleController controller(60000);
  TEST_ASSERT_TRUE(controller.tick(60000));
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(Role::Glyph), static_cast<uint8_t>(controller.role()));
  TEST_ASSERT_TRUE(controller.tick(120000));
  TEST_ASSERT_TRUE(controller.tick(180000));
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(Role::Cthulhu), static_cast<uint8_t>(controller.role()));
  TEST_ASSERT_TRUE(controller.tick(240000));
  TEST_ASSERT_TRUE(controller.seancePhase());
  TEST_ASSERT_EQUAL_UINT32(60000, controller.remainingMs(240000));
  TEST_ASSERT_TRUE(controller.tick(300000));
  TEST_ASSERT_FALSE(controller.seancePhase());
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(Role::Acolyte), static_cast<uint8_t>(controller.role()));
}

void setup() {
  delay(500);
  UNITY_BEGIN();
  RUN_TEST(test_automatic_mode_changes_role_at_60000_ms);
  RUN_TEST(test_manual_role_does_not_change_after_an_interval);
  RUN_TEST(test_automatic_mode_enters_seance_after_cthulhu_then_restarts_acolyte);
  UNITY_END();
}

void loop() {}
