#!/bin/sh
set -eu
cd "$(dirname "$0")"

# Explicit output names match the include in jl18853.trans.l.
"${BISON:-bison}" -d -o trans.tab.c jl18853.trans.y
"${FLEX:-flex}" -o lex.yy.c jl18853.trans.l
"${CXX:-g++}" -std=c++11 -x c++ lex.yy.c trans.tab.c treenode.cpp -o translator
