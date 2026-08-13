#include "acolyte_advertiser.h"

#include <NimBLEDevice.h>
#include <NimBLEExtAdvertising.h>

#include "role_profiles.h"

namespace {
NimBLEExtAdvertising* advertising = nullptr;

void stopStartedInstances(size_t started) {
  if (advertising == nullptr) return;
  for (size_t instance = 0; instance < started; ++instance) {
    advertising->stop(static_cast<uint8_t>(instance));
  }
}
}  // namespace

bool AcolyteAdvertiser::start(IdentityRotation& rotation) {
  advertising = NimBLEDevice::getAdvertising();
  if (advertising == nullptr) {
    lastError_ = "advertising unavailable";
    return false;
  }

  const size_t originalIndex = rotation.index(Role::Acolyte);
  size_t started = 0;
  for (size_t instance = 0; instance < kAcolytePeerCount; ++instance) {
    const RoleIdentity identity = rotation.current(Role::Acolyte);
    NimBLEExtAdvertisement packet;
    packet.setLegacyAdvertising(true);
    packet.setConnectable(false);
    packet.setScannable(false);
    packet.setFlags(0x06);
    packet.setAddress(NimBLEAddress(identity.address, BLE_ADDR_RANDOM));

    uint8_t data[20] = {};
    const size_t length = buildManufacturerData(Role::Acolyte, identity.name, data, sizeof(data));
    if (length == 0 || !packet.setManufacturerData(data, length)) {
      lastError_ = "packet construction failed";
    } else if (!advertising->setInstanceData(static_cast<uint8_t>(instance), packet)) {
      lastError_ = "instance configuration failed";
    } else if (!advertising->start(static_cast<uint8_t>(instance))) {
      lastError_ = "instance start failed";
    } else {
      names_[instance] = identity.name;
      rotation.advance(Role::Acolyte);
      started = instance + 1;
      continue;
    }

    stopStartedInstances(started);
    rotation.setIndex(Role::Acolyte, originalIndex);
    for (size_t clear = 0; clear < kAcolytePeerCount; ++clear) names_[clear] = "";
    active_ = false;
    return false;
  }

  active_ = true;
  lastError_ = "";
  return true;
}

// Clears state only. Instance teardown belongs to the caller, which tracks which
// instances are running; stopping them here as well would double-stop them and
// surface a spurious controller error. The failure path inside start() still
// stops what it started, because those instances are not yet tracked.
void AcolyteAdvertiser::stop() {
  for (size_t index = 0; index < kAcolytePeerCount; ++index) names_[index] = "";
  active_ = false;
}
bool AcolyteAdvertiser::active() const { return active_; }
const char* AcolyteAdvertiser::name(size_t index) const {
  return index < kAcolytePeerCount ? names_[index] : "";
}
const char* AcolyteAdvertiser::lastError() const { return lastError_; }
