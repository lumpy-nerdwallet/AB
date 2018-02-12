#!/bin/bash

echo "local consts = {}" >ab_consts.lua
cat ab_constants.h |grep "#define" | grep -v "__" | awk '{print $2"="$3}' | sed 's/\([a-zA-Z_]\+\)/consts\.\1/g' >> ab_consts.lua
echo "return consts" >> ab_consts.lua