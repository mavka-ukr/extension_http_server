#!/usr/bin/env sh

set -e
set -x

export CC="clang"
export CXX="clang++"
export AR="llvm-ar"
export RANLIB="llvm-ranlib"

cd uWebSockets
make capi
cd -

ціль розширення_мавки_http.ll скомпілювати розширення_мавки_http.ц
clang++ -shared -fPIC \
  -march=native \
  -std=c++20 \
  -flto \
  -o розширення_мавки_http.so розширення_мавки_http.ll http_mavka_extension.cpp \
  uWebSockets/uSockets/*.o \
  -IuWebSockets/src -IuWebSockets/uSockets/src
