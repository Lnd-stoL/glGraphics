rm core
cd ./build-debug-bin
cmake ..
make -j 4
cd ..
primusrun ./build-debug-bin/glGraphics
