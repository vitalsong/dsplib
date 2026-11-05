#!/bin/sh

conan create . -s build_type=Release
conan create . -s build_type=Debug

conan create . -s build_type=Release -o float32=True
conan create . -s build_type=Debug -o float32=True