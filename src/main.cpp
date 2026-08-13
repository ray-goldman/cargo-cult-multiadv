#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NimBLEDevice.h>
#include <NimBLEExtAdvertising.h>

#include "acolyte_advertiser.h"
#include "advertising_transition.h"
#include "config.h"
#include "normal_phase.h"
#include "role_controller.h"
#include "role_identities.h"
#include "role_profiles.h"
#include "seance_advertiser.h"
#include "seance_profiles.h"
#include "serial_commands.h"

namespace {
constexpr uint8_t kNormalInstanceId = 0;
constexpr uint8_t kSeanceInstanceMask = 0b0111;
Adafruit_SSD1306 display(128, 64, &Wire, -1);
RoleController controller(ROLE_INTERVAL_MS);
SeanceAdvertiser seance;
AcolyteAdvertiser acolytes;
AdvertisingTransition transition;
IdentityRotation rotation;
NimBLEExtAdvertising* advertising = nullptr;
uint8_t activeInstances = 0;
bool oledOk = false;
bool bleOk = false;
// Name of the identity currently on air, for status output.
const char* activeName = "";
char line[96] = {};
size_t used = 0;
uint32_t displayedSeconds = UINT32_MAX;

bool advertiseNormalRole() {
  if (advertising == nullptr) return false;
  const Role role = controller.role();
  const RoleIdentity identity = rotation.current(role);

  NimBLEExtAdvertisement packet;
  packet.setLegacyAdvertising(true);
  packet.setConnectable(false);
  packet.setScannable(false);
  packet.setFlags(0x06);
  // A target badge keys members by peer identity, so every role must present
  // its own address. Setting it explicitly also prevents instance 0 from
  // inheriting whatever address the previous Seance phase configured.
  packet.setAddress(NimBLEAddress(identity.address, BLE_ADDR_RANDOM));

  uint8_t data[20] = {};
  const size_t length = buildManufacturerData(role, identity.name, data, sizeof(data));
  if (length == 0 || !packet.setManufacturerData(data, length)) return false;
  if (!advertising->setInstanceData(kNormalInstanceId, packet)) return false;
  if (!advertising->start(kNormalInstanceId)) return false;

  // Only advance once the identity has actually gone on air.
  activeName = identity.name;
  rotation.advance(role);
  return true;
}

uint32_t secondsRemaining() {
  return controller.automatic() ? (controller.remainingMs(millis()) + 999) / 1000 : 0;
}

void render() {
  if (!oledOk) return;
  displayedSeconds = secondsRemaining();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Cargo Cult Multi-Adv");
  display.setCursor(0, 18);
  if (seance.active()) {
    display.print("SEANCE: 3 peers");
  } else if (acolytes.active()) {
    display.print("Acolyte x3");
  } else {
    display.print(profileFor(controller.role()).displayName);
    display.print(" ");
    display.print(activeName);
  }
  display.setCursor(0, 34);
  display.print(controller.automatic() ? "AUTO " : "MANUAL ");
  if (controller.automatic()) {
    display.print(displayedSeconds);
    display.print(" sec");
  } else if (seance.active()) {
    display.print("3 peers");
  }
  display.setCursor(0, 50);
  if (transition.pending()) {
    display.print("BLE: SWITCH");
  } else {
    display.print(bleOk ? "BLE: ADV" : "BLE: ERR");
  }
  display.display();
}

void status() {
  if (transition.pending()) {
    Serial.print("ble=SWITCH remaining=");
    Serial.println(secondsRemaining());
    return;
  }
  if (seance.active()) {
    Serial.print("mode=");
    Serial.print(controller.automatic() ? "AUTO-SEANCE" : "SEANCE");
    Serial.print(" peers=3 set=");
    Serial.print(seanceSetIndex());
    Serial.print("/");
    Serial.print(seanceSetCount());
    Serial.print(" remaining=");
    Serial.println(controller.remainingMs(millis()));
    return;
  }
  if (acolytes.active()) {
    Serial.print("role=Acolyte names=");
    for (size_t index = 0; index < kAcolytePeerCount; ++index) {
      if (index) Serial.print(",");
      Serial.print(acolytes.name(index));
    }
    Serial.print(" next=");
    Serial.print(rotation.index(Role::Acolyte));
    Serial.print("/");
    Serial.print(roleIdentityCount(Role::Acolyte));
    Serial.print(" mode=");
    Serial.print(controller.automatic() ? "AUTO" : "MANUAL");
    Serial.print(" remaining=");
    Serial.println(controller.remainingMs(millis()));
    return;
  }
  Serial.print("role=");
  Serial.print(profileFor(controller.role()).displayName);
  Serial.print(" name=");
  Serial.print(activeName);
  Serial.print(" next=");
  Serial.print(rotation.index(controller.role()));
  Serial.print("/");
  Serial.print(roleIdentityCount(controller.role()));
  Serial.print(" mode=");
  Serial.print(controller.automatic() ? "AUTO" : "MANUAL");
  Serial.print(" remaining=");
  Serial.println(controller.remainingMs(millis()));
}

void queueAdvertising(AdvertisingTarget target) {
  // The advertisers only clear their own state here. This function is the sole
  // owner of stopping advertising instances, tracked by activeInstances; having
  // an advertiser stop them too would make the loop below report a controller
  // error for an instance that was already stopped.
  if (target == AdvertisingTarget::Normal) seance.stop();
  acolytes.stop();
  const bool needsSettlement = activeInstances != 0;
  transition.request(target, needsSettlement, millis());
  for (uint8_t instance = 0; instance < 3; ++instance) {
    if (activeInstances & (1U << instance)) {
      if (advertising->stop(instance)) {
        activeInstances &= static_cast<uint8_t>(~(1U << instance));
      } else {
        Serial.print("ble=ERROR stop instance=");
        Serial.println(instance);
      }
    }
  }
  render();
}

void recoverAcolyte() {
  bleOk = false;
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);
  controller.setAutomatic(millis());
  queueAdvertising(AdvertisingTarget::Normal);
}

void serviceAdvertising() {
  if (!transition.ready(millis())) return;
  const AdvertisingTarget target = transition.target();
  transition.reset();
  bool started = false;
  if (target == AdvertisingTarget::Normal) {
    if (controller.role() == Role::Acolyte) {
      started = acolytes.start(rotation);
    } else {
      started = advertiseNormalRole();
    }
    if (started) activeInstances = normalInstanceMask(controller.role());
  } else {
    started = seance.start();
    if (started) activeInstances = kSeanceInstanceMask;
  }
  bleOk = started;
  digitalWrite(LED_GREEN, started ? HIGH : LOW);
  digitalWrite(LED_RED, started ? LOW : HIGH);
  if (!started) {
    Serial.println("ble=ERROR target start failed");
    recoverAcolyte();
  } else if (target == AdvertisingTarget::Seance) {
    Serial.println("seance=ACTIVE peers=3");
  }
  render();
  status();
}

void applyAutomaticPhase() {
  queueAdvertising(controller.seancePhase() ? AdvertisingTarget::Seance : AdvertisingTarget::Normal);
}

void handleCommand() {
  line[used] = 0;
  const Command command = parseCommand(line);
  if (command.kind == CommandKind::Status) {
    status();
    return;
  }
  if (command.kind == CommandKind::Help) {
    Serial.println("help status mode auto|manual|seance role acolyte|glyph|elder|cthulhu next");
    return;
  }
  if (command.kind == CommandKind::Invalid) {
    Serial.println("invalid command");
    return;
  }
  if (command.kind == CommandKind::ModeSeance) {
    controller.setManual(controller.role(), millis());
    queueAdvertising(AdvertisingTarget::Seance);
    return;
  }

  if (command.kind == CommandKind::ModeAuto) {
    controller.setAutomatic(millis());
  } else if (command.kind == CommandKind::ModeManual) {
    controller.setManual(controller.role(), millis());
  } else if (command.kind == CommandKind::SelectRole) {
    controller.setManual(command.role, millis());
  } else if (command.kind == CommandKind::Next) {
    controller.next(millis());
  }
  queueAdvertising(AdvertisingTarget::Normal);
}
}  // namespace

void setup() {
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);

  Serial.begin(SERIAL_BAUD);
  Wire.begin(OLED_SDA, OLED_SCL);
  oledOk = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (!oledOk) digitalWrite(LED_RED, HIGH);

  NimBLEDevice::init("");
  advertising = NimBLEDevice::getAdvertising();
  queueAdvertising(AdvertisingTarget::Normal);
  serviceAdvertising();
  Serial.println("Cult role emulator ready");
  status();
}

void loop() {
  serviceAdvertising();
  if (controller.automatic() && controller.tick(millis())) {
    applyAutomaticPhase();
  }
  if (controller.automatic() && secondsRemaining() != displayedSeconds) render();

  while (Serial.available()) {
    const char character = static_cast<char>(Serial.read());
    if (character == '\r' || character == '\n') {
      if (used) handleCommand();
      used = 0;
    } else if (used < sizeof(line) - 1) {
      line[used++] = character;
    }
  }
}
