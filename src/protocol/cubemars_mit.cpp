#include "cubemars_mit.hpp"
#include <cstdint>
namespace {

float uintToFloat(unsigned int value, float min, float max, unsigned int bits) {
  const float span = max - min;
  const unsigned int max_int = (1U << bits) - 1U;

  return static_cast<float>(value) * span / static_cast<float>(max_int) + min;
}

}  // namespace
MitSpecialCommand detectMitSpecialCommand(const can_frame& frame) {
  if (frame.can_dlc != 8) {
    return MitSpecialCommand::None;
  }

  for (int i = 0; i < 7; ++i) {
    if (frame.data[i] != 0xFF) {
      return MitSpecialCommand::None;
    }
  }

  switch (frame.data[7]) {
    case 0xFC:
      return MitSpecialCommand::EnterMotorMode;

    case 0xFD:
      return MitSpecialCommand::ExitMotorMode;

    case 0xFE:
      return MitSpecialCommand::SetZero;

    default:
      return MitSpecialCommand::None;
  }
}

const char* toString(MitSpecialCommand command) {
  switch (command) {
    case MitSpecialCommand::EnterMotorMode:
      return "ENTER MOTOR MODE";

    case MitSpecialCommand::ExitMotorMode:
      return "EXIT MOTOR MODE";

    case MitSpecialCommand::SetZero:
      return "SET ZERO";

    case MitSpecialCommand::None:
    default:
      return "NONE";
  }
}

bool decodeMitCommand(const can_frame& frame, MitCommand& command) {
  if (frame.can_dlc != 8) {
    return false;
  }

  if (detectMitSpecialCommand(frame) != MitSpecialCommand::None) {
    return false;
  }

  const unsigned int p_int = (static_cast<unsigned int>(frame.data[0]) << 8) | frame.data[1];

  const unsigned int v_int = (static_cast<unsigned int>(frame.data[2]) << 4) | (frame.data[3] >> 4);

  const unsigned int kp_int =
      ((static_cast<unsigned int>(frame.data[3]) & 0x0F) << 8) | frame.data[4];

  const unsigned int kd_int =
      (static_cast<unsigned int>(frame.data[5]) << 4) | (frame.data[6] >> 4);

  const unsigned int t_int =
      ((static_cast<unsigned int>(frame.data[6]) & 0x0F) << 8) | frame.data[7];

  constexpr float P_MIN = -12.5f;
  constexpr float P_MAX = 12.5f;

  constexpr float V_MIN = -45.0f;
  constexpr float V_MAX = 45.0f;

  constexpr float KP_MIN = 0.0f;
  constexpr float KP_MAX = 500.0f;

  constexpr float KD_MIN = 0.0f;
  constexpr float KD_MAX = 5.0f;

  constexpr float T_MIN = -15.0f;
  constexpr float T_MAX = 15.0f;

  command.position = uintToFloat(p_int, P_MIN, P_MAX, 16);
  command.velocity = uintToFloat(v_int, V_MIN, V_MAX, 12);
  command.kp = uintToFloat(kp_int, KP_MIN, KP_MAX, 12);
  command.kd = uintToFloat(kd_int, KD_MIN, KD_MAX, 12);
  command.torque = uintToFloat(t_int, T_MIN, T_MAX, 12);

  return true;
}

unsigned int floatToUint(float value,
                         float min,
                         float max,
                         unsigned int bits) {
  if (value < min) {
    value = min;
  } else if (value > max) {
    value = max;
  }

  const float span = max - min;
  const unsigned int max_int = (1U << bits) - 1U;

  return static_cast<unsigned int>(
      (value - min) * static_cast<float>(max_int) / span);
}

bool encodeMitCommand(const MitCommand& command, can_frame& frame) {
  constexpr float P_MIN = -12.5f;
  constexpr float P_MAX = 12.5f;

  constexpr float V_MIN = -45.0f;
  constexpr float V_MAX = 45.0f;

  constexpr float KP_MIN = 0.0f;
  constexpr float KP_MAX = 500.0f;

  constexpr float KD_MIN = 0.0f;
  constexpr float KD_MAX = 5.0f;

  constexpr float T_MIN = -15.0f;
  constexpr float T_MAX = 15.0f;

  const unsigned int p_int =
      floatToUint(command.position, P_MIN, P_MAX, 16);

  const unsigned int v_int =
      floatToUint(command.velocity, V_MIN, V_MAX, 12);

  const unsigned int kp_int =
      floatToUint(command.kp, KP_MIN, KP_MAX, 12);

  const unsigned int kd_int =
      floatToUint(command.kd, KD_MIN, KD_MAX, 12);

  const unsigned int t_int =
      floatToUint(command.torque, T_MIN, T_MAX, 12);

  frame.can_dlc = 8;

  frame.data[0] = static_cast<uint8_t>(p_int >> 8);
  frame.data[1] = static_cast<uint8_t>(p_int & 0xFF);

  frame.data[2] = static_cast<uint8_t>(v_int >> 4);

  frame.data[3] =
      static_cast<uint8_t>(((v_int & 0x0F) << 4) |
                           (kp_int >> 8));

  frame.data[4] = static_cast<uint8_t>(kp_int & 0xFF);

  frame.data[5] = static_cast<uint8_t>(kd_int >> 4);

  frame.data[6] =
      static_cast<uint8_t>(((kd_int & 0x0F) << 4) |
                           (t_int >> 8));

  frame.data[7] = static_cast<uint8_t>(t_int & 0xFF);

  return true;
}