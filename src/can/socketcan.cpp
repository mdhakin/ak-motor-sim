#include "socketcan.hpp"

#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

SocketCan::SocketCan(const std::string& interface_name) {
  socket_fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);

  if (socket_fd_ < 0) {
    throw std::runtime_error("Failed to create CAN socket");
  }

  ifreq ifr{};
  std::strncpy(ifr.ifr_name, interface_name.c_str(), IFNAMSIZ - 1);

  if (ioctl(socket_fd_, SIOCGIFINDEX, &ifr) < 0) {
    close(socket_fd_);
    throw std::runtime_error("Failed to find CAN interface: " + interface_name);
  }

  sockaddr_can addr{};
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  if (bind(socket_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    close(socket_fd_);
    throw std::runtime_error("Failed to bind CAN socket");
  }
}

SocketCan::~SocketCan() {
  if (socket_fd_ >= 0) {
    close(socket_fd_);
  }
}

bool SocketCan::receive(can_frame& frame) {
  const ssize_t bytes_read = read(socket_fd_, &frame, sizeof(frame));

  return bytes_read == sizeof(frame);
}