#include <Arduino.h>
#include <unity.h>

#include "serial_commands.h"

void test_role_command_is_case_insensitive() {
  const Command command = parseCommand("role ELDER");
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(CommandKind::SelectRole), static_cast<uint8_t>(command.kind));
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(Role::Elder), static_cast<uint8_t>(command.role));
}

void test_unknown_command_is_invalid() {
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(CommandKind::Invalid),
                          static_cast<uint8_t>(parseCommand("erase").kind));
}

void test_seance_command_is_case_insensitive() {
  const Command command = parseCommand("mode SEANCE");
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(CommandKind::ModeSeance),
                          static_cast<uint8_t>(command.kind));
}

void setup() {
  delay(500);
  UNITY_BEGIN();
  RUN_TEST(test_role_command_is_case_insensitive);
  RUN_TEST(test_unknown_command_is_invalid);
  RUN_TEST(test_seance_command_is_case_insensitive);
  UNITY_END();
}

void loop() {}
