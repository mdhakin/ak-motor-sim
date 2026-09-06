#!/usr/bin/env bash

set -e

INTERFACE="vcan0"

echo "Setting up ${INTERFACE}..."

# Load the virtual CAN kernel module.
sudo modprobe vcan

# Create vcan0 if it does not already exist.
if ! ip link show "${INTERFACE}" > /dev/null 2>&1; then
    sudo ip link add dev "${INTERFACE}" type vcan
    echo "Created ${INTERFACE}"
else
    echo "${INTERFACE} already exists"
fi

# Bring the interface up.
sudo ip link set up "${INTERFACE}"

echo
echo "${INTERFACE} is ready:"
ip link show "${INTERFACE}"
