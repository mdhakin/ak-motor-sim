#pragma once

#include <linux/can.h>

#include <string>

class SocketCan {
 public:
  explicit SocketCan(const std::string& interface_name);
  ~SocketCan();

  SocketCan(const SocketCan&) = delete;
  SocketCan& operator=(const SocketCan&) = delete;

  bool receive(can_frame& frame);

 private:
  int socket_fd_ = -1;
};