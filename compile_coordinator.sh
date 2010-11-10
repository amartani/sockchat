#!/bin/sh
set -e

gcc -g coordinator.c -o coordinator -pthread -lm