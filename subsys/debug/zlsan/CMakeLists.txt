# Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
#
# SPDX-License-Identifier: Apache-2.0
#

add_subdirectory(llvm_pass)

zephyr_interface_library_named(zlsan)

zephyr_library()

zephyr_library_sources(
  zlsan.cpp
  zlsan_k_mem_pool.cpp
)

zephyr_library_link_libraries(zlsan)
