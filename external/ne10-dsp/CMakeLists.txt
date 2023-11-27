#
#  Copyright 2011-16 ARM Limited and Contributors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the name of ARM Limited nor the
#      names of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY ARM LIMITED AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL ARM LIMITED AND CONTRIBUTORS BE LIABLE FOR ANY
#  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0077 NEW)

project(ne10dsp LANGUAGES C)

set(NE10_VERSION 10)

# Make sure we are compiling for an ARM system
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch")
    message(FATAL_ERROR "You are trying to compile for non-ARM (CMAKE_SYSTEM_PROCESSOR='${CMAKE_SYSTEM_PROCESSOR}')!")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon -funsafe-math-optimizations -std=gnu99")

set(NE10_DSP_C_SRCS
    ${PROJECT_SOURCE_DIR}/modules/dsp/NE10_fft.c
    ${PROJECT_SOURCE_DIR}/modules/dsp/NE10_fft_float32.c
    ${PROJECT_SOURCE_DIR}/modules/dsp/NE10_fft_generic_float32.c
    ${PROJECT_SOURCE_DIR}/modules/dsp/NE10_rfft_float32.c
)

set(NE10_DSP_INTRINSIC_SRCS
    ${PROJECT_SOURCE_DIR}/modules/dsp/NE10_fft_float32.neonintrinsic.c
    ${PROJECT_SOURCE_DIR}/modules/dsp/NE10_rfft_float32.neonintrinsic.c
    ${PROJECT_SOURCE_DIR}/modules/dsp/NE10_fft_generic_float32.neonintrinsic.cpp
)

set(NE10_DSP_INIT_SRCS
    ${PROJECT_SOURCE_DIR}/modules/dsp/NE10_init_dsp.c
    ${PROJECT_SOURCE_DIR}/modules/NE10_init.c
)

include_directories (
    ${PROJECT_SOURCE_DIR}/inc
    ${PROJECT_SOURCE_DIR}/common
)

add_library(${PROJECT_NAME} STATIC
    ${NE10_DSP_C_SRCS}
    ${NE10_DSP_INTRINSIC_SRCS}
    ${NE10_DSP_INIT_SRCS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    -DNE10_ENABLE_DSP
    -DNE10_ENABLE_HF
    -DNE10_UNROLL_LEVEL=1
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/inc
    ${PROJECT_SOURCE_DIR}/common
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CLEAN_DIRECT_OUTPUT 1
    VERSION ${NE10_VERSION}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    LINKER_LANGUAGE C
)
