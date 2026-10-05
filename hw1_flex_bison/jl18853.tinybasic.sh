#!/bin/sh
set -e
cd "$(dirname "$0")"

sh ./jl18853.trans.sh
./translator < jl18853.tinybasic.in > jl18853.tinybasic.y

bison -d -o jl18853.tinybasic.tab.c jl18853.tinybasic.y
flex -o jl18853.tinybasic.yy.c jl18853.tinybasic.l
"${CXX:-g++}" -std=c++11 -x c++ \
    jl18853.tinybasic.yy.c jl18853.tinybasic.tab.c -o tinybasic
