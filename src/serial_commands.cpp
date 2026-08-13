#include "serial_commands.h"

#include <ctype.h>
#include <string.h>

Command parseCommand(const char* input) {
  char buffer[32] = {};
  if (!input) return {CommandKind::Invalid, Role::Acolyte};
  size_t length = strnlen(input, sizeof(buffer) - 1);
  memcpy(buffer, input, length);
  while (length && isspace(static_cast<unsigned char>(buffer[length - 1]))) buffer[--length] = 0;
  size_t start = 0;
  while (isspace(static_cast<unsigned char>(buffer[start]))) ++start;
  for (size_t i = start; i < length; ++i) buffer[i] = tolower(static_cast<unsigned char>(buffer[i]));
  const char* command = buffer + start;

  if (!strcmp(command, "help")) return {CommandKind::Help, Role::Acolyte};
  if (!strcmp(command, "status")) return {CommandKind::Status, Role::Acolyte};
  if (!strcmp(command, "mode auto")) return {CommandKind::ModeAuto, Role::Acolyte};
  if (!strcmp(command, "mode manual")) return {CommandKind::ModeManual, Role::Acolyte};
  if (!strcmp(command, "mode seance")) return {CommandKind::ModeSeance, Role::Acolyte};
  if (!strcmp(command, "next")) return {CommandKind::Next, Role::Acolyte};
  if (!strncmp(command, "role ", 5)) {
    const char* role = command + 5;
    if (!strcmp(role, "acolyte")) return {CommandKind::SelectRole, Role::Acolyte};
    if (!strcmp(role, "glyph")) return {CommandKind::SelectRole, Role::Glyph};
    if (!strcmp(role, "elder")) return {CommandKind::SelectRole, Role::Elder};
    if (!strcmp(role, "cthulhu")) return {CommandKind::SelectRole, Role::Cthulhu};
  }
  return {CommandKind::Invalid, Role::Acolyte};
}
