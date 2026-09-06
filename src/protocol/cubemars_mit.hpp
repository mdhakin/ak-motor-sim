#pragma once

#include <linux/can.h>

enum class MitSpecialCommand { None, EnterMotorMode, ExitMotorMode, SetZero };

struct MitCommand {
  float position;
  float velocity;
  float kp;
  float kd;
  float torque;
};

MitSpecialCommand detectMitSpecialCommand(const can_frame& frame);

bool decodeMitCommand(const can_frame& frame, MitCommand& command);
bool encodeMitCommand(const MitCommand& command, can_frame& frame);

const char* toString(MitSpecialCommand command);