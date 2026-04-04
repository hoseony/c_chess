#/usr/bin/bash

gcc ui.c -L../../lib -lraylib -I../../include -lX11 -lm -o ui
