#!/usr/bin/env sh

set -e
set -x

ціль розширення_мавки_http_server.ll скомпілювати розширення_мавки_http_server.ц
clang -shared -fPIC -o розширення_мавки_http_server.so розширення_мавки_http_server.ll http_server_mavka_extension.c
