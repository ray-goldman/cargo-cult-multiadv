#pragma once
#include "role_profiles.h"
enum class CommandKind : uint8_t { Help, Status, ModeAuto, ModeManual, ModeSeance, SelectRole, Next, Invalid };
struct Command { CommandKind kind; Role role; };
Command parseCommand(const char* text);
