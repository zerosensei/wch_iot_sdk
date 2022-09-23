# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(WCH REQUIRED HINTS $ENV{WCH_BASE})

project(button)

target_sources(app PRIVATE src/main.c)