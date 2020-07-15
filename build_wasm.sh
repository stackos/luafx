#! /bin/bash
if [ ! -d "build/wasm" ]; then
    mkdir -p build/wasm
fi
cd build/wasm
emcmake cmake ../../ -DTarget=WASM
emmake make
cp -rf ../../assets ./
cp -f ../../src/app/platform/web/index.html index.html
cp -f ../../src/app/platform/web/index.js index.js
emcc libluafx.a libluafx_wasm.a \
    -o luafx.js \
    -s NO_EXIT_RUNTIME=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' -s ALLOW_MEMORY_GROWTH=1 \
    -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s USE_WEBGL2=0 \
    --preload-file assets \
    -O3 -g4
cd ../../
