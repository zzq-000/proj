#include "ge_channel.h"

bool GEChannel::IsLossThisRound()
{
  float drop_draw = distribution_(generator_);
  if (bad_state_) {
    return drop_draw <= p_loss_bad_;
  }
  else {
    return drop_draw <= p_loss_good_;
  }
}

bool GEChannel::IsReceiveThisRound() {
  return !IsLossThisRound();
}

void GEChannel::Transition()
{
  float transition_draw = distribution_(generator_);
  if (bad_state_) {
    if (transition_draw <= p_bad_to_good_) { bad_state_ = false; }
  }
  else {
    if (transition_draw <= p_good_to_bad_) { bad_state_ = true; }
  }
}