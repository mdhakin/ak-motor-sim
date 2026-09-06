#include <iomanip>
#include <iostream>

#include "can/socketcan.hpp"
#include "protocol/cubemars_mit.hpp"

int main() {
  try {
    SocketCan can("vcan0");
    MitCommand test_command{
        .position = 0.0f,
        .velocity = 3.0f,
        .kp = 0.0f,
        .kd = 2.5f,
        .torque = 0.0f,
    };

    can_frame test_frame{};

    encodeMitCommand(test_command, test_frame);

    std::cout << "Encoded test command:\n";
    std::cout << "DATA=";

    for (int i = 0; i < test_frame.can_dlc; ++i) {
      std::cout << std::hex
                << std::uppercase
                << std::setw(2)
                << std::setfill('0')
                << static_cast<int>(test_frame.data[i])
                << ' ';
    }
 
    MitCommand decoded_command{};

if (decodeMitCommand(test_frame, decoded_command)) {
  std::cout << std::fixed << std::setprecision(4);

  std::cout << "Round-trip result:\n";
  std::cout << "P  = " << decoded_command.position << '\n';
  std::cout << "V  = " << decoded_command.velocity << '\n';
  std::cout << "Kp = " << decoded_command.kp << '\n';
  std::cout << "Kd = " << decoded_command.kd << '\n';
  std::cout << "T  = " << decoded_command.torque << "\n\n";
}

std::cout << std::dec << "\n\n";
    std::cout << "Listening on vcan0...\n";

    while (true) {
      can_frame frame{};

      if (!can.receive(frame)) {
        continue;
      }

      std::cout << "RX ID=0x" << std::hex << std::uppercase << (frame.can_id & CAN_EFF_MASK)
                << std::dec << " DLC=" << static_cast<int>(frame.can_dlc) << " DATA=";

      for (int i = 0; i < frame.can_dlc; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(frame.data[i]) << ' ';
      }

      std::cout << std::dec << '\n';

      const auto special_command = detectMitSpecialCommand(frame);

      if (special_command != MitSpecialCommand::None) {
        std::cout << "  -> " << toString(special_command) << '\n';
      }

      MitCommand command{};

      if (decodeMitCommand(frame, command)) {
        std::cout << std::fixed << std::setprecision(3);

        std::cout << "  P  = " << command.position << " rad\n";
        std::cout << "  V  = " << command.velocity << " rad/s\n";
        std::cout << "  Kp = " << command.kp << '\n';
        std::cout << "  Kd = " << command.kd << '\n';
        std::cout << "  T  = " << command.torque << " Nm\n";
      }
    }

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}