cd ~/src

git clone --recurse-submodules git@github.com:mdhakin/ak-motor-sim.git

cd ak-motor-sim

cmake -S . -B build -G Ninja
cmake --build build

./setup_vcan.sh
./build/ak-motor-sim