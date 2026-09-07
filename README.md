sudo apt update
sudo apt install -y can-utils

sudo modprobe can
sudo modprobe can_raw
sudo modprobe vcan

sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0

ip link show vcan0


cd ~/src

git clone --recurse-submodules git@github.com:mdhakin/ak-motor-sim.git

cd ak-motor-sim

cmake -S . -B build -G Ninja
cmake --build build

./setup_vcan.sh
./build/ak-motor-sim