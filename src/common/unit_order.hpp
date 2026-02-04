#ifndef GDEXTENSION_UNIT_ORDER_H
#define GDEXTENSION_UNIT_ORDER_H

enum class OrderType {
  NONE,
  MOVE,
  ATTACK,
  CHASE,  // Chase a target (move toward them without attacking)
  INTERACT,
};

#endif  // GDEXTENSION_UNIT_ORDER_H
