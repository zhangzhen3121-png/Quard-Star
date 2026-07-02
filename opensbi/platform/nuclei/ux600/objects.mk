#
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (c) 2020 Nuclei Corporation or its affiliates.
#
# Authors:
#   lujun <lujun@nuclesys.com>
#   hqfang <578567190@qq.com>
#

# Compiler flags
platform-cppflags-y =
platform-cflags-y =
platform-asflags-y =
platform-ldflags-y =

# Command for platform specific "make run"
platform-runcmd = xl_spike \
  $(build_dir)/platform/nuclei/ux600/firmware/fw_payload.elf

# Objects to build
platform-objs-y += platform.o

# Blobs to build
FW_JUMP=y
FW_TEXT_ADDR=0x0
FW_TEXT_START=0x80000000




