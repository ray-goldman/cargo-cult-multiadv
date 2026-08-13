#pragma once

#include <stddef.h>
#include <stdint.h>

#include "role_profiles.h"

struct SeancePeer {
  Role role;
  const char* name;
  const uint8_t* address;
};

// Number of distinct three-peer sets the Seance rotates through. Set 0 is the
// combination verified against the original badge; later sets reuse the same
// structure with fresh addresses so repeated Seance phases register as new
// peers rather than as already-known members.
constexpr size_t kSeancePeerSets = 4;

const SeancePeer* seancePeers();
size_t seancePeerCount();

// Advances to the next peer set. Called after a Seance starts successfully.
void advanceSeancePeers();
size_t seanceSetIndex();
size_t seanceSetCount();
void resetSeancePeers();
