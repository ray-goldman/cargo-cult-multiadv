#include "role_controller.h"

RoleController::RoleController(uint32_t intervalMs)
    : role_(Role::Acolyte), interval_(intervalMs), last_(0), auto_(true), seance_(false) {}

Role RoleController::role() const { return role_; }
bool RoleController::automatic() const { return auto_; }
bool RoleController::seancePhase() const { return seance_; }

uint32_t RoleController::remainingMs(uint32_t now) const {
  return auto_ ? (now - last_ >= interval_ ? 0 : interval_ - (now - last_)) : 0;
}

bool RoleController::tick(uint32_t now) {
  if (!auto_ || now - last_ < interval_) return false;
  do {
    if (seance_) {
      seance_ = false;
      role_ = Role::Acolyte;
    } else if (role_ == Role::Cthulhu) {
      seance_ = true;
    } else {
      role_ = nextRole(role_);
    }
    last_ += interval_;
  } while (now - last_ >= interval_);
  return true;
}

void RoleController::setAutomatic(uint32_t now) {
  role_ = Role::Acolyte;
  auto_ = true;
  seance_ = false;
  last_ = now;
}

void RoleController::setManual(Role role, uint32_t now) {
  role_ = role;
  auto_ = false;
  seance_ = false;
  last_ = now;
}

void RoleController::next(uint32_t now) {
  role_ = nextRole(role_);
  auto_ = false;
  seance_ = false;
  last_ = now;
}
