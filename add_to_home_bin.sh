#!/usr/bin/bash


find "$(pwd)" -maxdepth 1 \( -type f -o -type l \) -executable -exec ln -s -t ~/bin {} \;
