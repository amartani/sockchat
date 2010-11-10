#!/bin/sh
set -e

gcc -g server.c -o server.out -pthread
py.test

