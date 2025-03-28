eval emcmake cmake -B ./build_wasm -DEMSCRIPTEN=1 -DCMAKE_BUILD_TYPE=Release
eval emmake make -C build_wasm -j 12
