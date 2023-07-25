#!/bin/bash

cd lxr  && ./linux_build.sh && cd ..
cd prsr && ./linux_build.sh && cd ..
cd expr && ./linux_build.sh && cd ..
cd chckr && ./linux_build.sh && cd ..
cd cfg && ./linux_build.sh && cd ..
 
FLAGS="-O3 -Wall -Wextra -o app "
clang main.c $FLAGS