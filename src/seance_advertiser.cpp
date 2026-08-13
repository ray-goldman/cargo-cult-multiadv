#include "seance_advertiser.h"

#include <NimBLEDevice.h>
#include <NimBLEExtAdvertising.h>

#include <string>

#include "role_profiles.h"
#include "seance_profiles.h"

namespace {
constexpr uint8_t kFirstInstanceId = 0;
NimBLEExtAdvertising* advertising = nullptr;

// Stops the instances configured so far, so a partial start does not leave
// stray advertisements running.
void stopStartedInstances(size_t started) {
  if (advertising == nullptr) return;
  for (size_t index = 0; index < started; ++index) {
    advertising->stop(static_cast<uint8_t>(kFirstInstanceId + index));
  }
}
}  // namespace

bool SeanceAdvertiser::begin() {
  advertising = NimBLEDevice::getAdvertising();
  if (advertising == nullptr) {
    lastError_ = "advertising unavailable";
    return false;
  }
  return true;
}

bool SeanceAdvertiser::start() {
  if (!begin()) return false;
  stop();

  const SeancePeer* peers = seancePeers();
  size_t started = 0;
  for (size_t index = 0; index < seancePeerCount(); ++index) {
    NimBLEExtAdvertisement packet;
    packet.setLegacyAdvertising(true);
    packet.setConnectable(false);
    packet.setScannable(false);
    packet.setFlags(0x06);
    packet.setAddress(NimBLEAddress(peers[index].address, BLE_ADDR_RANDOM));

    uint8_t data[20] = {};
    // Seance peers keep the SEAN tag; only the Seance parser accepts it.
    const size_t length =
        buildManufacturerData(peers[index].role, peers[index].name, data, sizeof(data), kSeanceTag);
    if (length == 0 || !packet.setManufacturerData(data, length)) {
      stopStartedInstances(started);
      active_ = false;
      lastError_ = "packet construction failed";
      return false;
    }
    if (!advertising->setInstanceData(kFirstInstanceId + index, packet)) {
      stopStartedInstances(started);
      active_ = false;
      lastError_ = "instance configuration failed";
      return false;
    }
    if (!advertising->start(kFirstInstanceId + index)) {
      stopStartedInstances(started);
      active_ = false;
      lastError_ = "instance start failed";
      return false;
    }
    started = index + 1;
  }

  active_ = true;
  lastError_ = "";
  // Move to the next peer set so the following Seance presents new identities.
  advanceSeancePeers();
  return true;
}

// Clears state only. Instance teardown belongs to the caller, which tracks which
// instances are running; stopping them here as well would double-stop them and
// surface a spurious controller error.
void SeanceAdvertiser::stop() {
  active_ = false;
}

bool SeanceAdvertiser::active() const { return active_; }
const char* SeanceAdvertiser::lastError() const { return lastError_; }
