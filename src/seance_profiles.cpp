#include "seance_profiles.h"

namespace {

// Set 0 holds the addresses and names captured in docs/ble-capture.md, so the
// first Seance after a reset behaves exactly as previously verified. Byte 4
// distinguishes the set and byte 5 the role.
constexpr uint8_t kAddresses[kSeancePeerSets][3][6] = {
    {{0xc2, 0x54, 0x53, 0x45, 0x41, 0x02},
     {0xc2, 0x54, 0x53, 0x45, 0x41, 0x03},
     {0xc2, 0x54, 0x53, 0x45, 0x41, 0x04}},
    {{0xc2, 0x54, 0x53, 0x45, 0x42, 0x02},
     {0xc2, 0x54, 0x53, 0x45, 0x42, 0x03},
     {0xc2, 0x54, 0x53, 0x45, 0x42, 0x04}},
    {{0xc2, 0x54, 0x53, 0x45, 0x43, 0x02},
     {0xc2, 0x54, 0x53, 0x45, 0x43, 0x03},
     {0xc2, 0x54, 0x53, 0x45, 0x43, 0x04}},
    {{0xc2, 0x54, 0x53, 0x45, 0x44, 0x02},
     {0xc2, 0x54, 0x53, 0x45, 0x44, 0x03},
     {0xc2, 0x54, 0x53, 0x45, 0x44, 0x04}},
};

constexpr SeancePeer kPeerSets[kSeancePeerSets][3] = {
    {{Role::Glyph, "SEANGLYPH1", kAddresses[0][0]},
     {Role::Elder, "SEANELDER1", kAddresses[0][1]},
     {Role::Cthulhu, "SEANCTHUL1", kAddresses[0][2]}},
    // Sets 1-3 stay within seven characters so they remain distinguishable in
    // the badge's member list, which truncates stored names at seven.
    {{Role::Glyph, "DEADC0D", kAddresses[1][0]},
     {Role::Elder, "0FFGRID", kAddresses[1][1]},
     {Role::Cthulhu, "WH1SPR_", kAddresses[1][2]}},
    {{Role::Glyph, "N3TB0T_", kAddresses[2][0]},
     {Role::Elder, "SP00FER", kAddresses[2][1]},
     {Role::Cthulhu, "3XPL0IT", kAddresses[2][2]}},
    {{Role::Glyph, "C0BALT_", kAddresses[3][0]},
     {Role::Elder, "V1RUS_X", kAddresses[3][1]},
     {Role::Cthulhu, "C4CH3D_", kAddresses[3][2]}},
};

size_t setIndex = 0;

}  // namespace

const SeancePeer* seancePeers() { return kPeerSets[setIndex]; }

size_t seancePeerCount() { return sizeof(kPeerSets[0]) / sizeof(kPeerSets[0][0]); }

void advanceSeancePeers() { setIndex = (setIndex + 1) % kSeancePeerSets; }

size_t seanceSetIndex() { return setIndex; }

size_t seanceSetCount() { return kSeancePeerSets; }

void resetSeancePeers() { setIndex = 0; }
