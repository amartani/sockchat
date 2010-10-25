#!/bin/sh
set -e

gcc server.c -o server.out
nosetests
