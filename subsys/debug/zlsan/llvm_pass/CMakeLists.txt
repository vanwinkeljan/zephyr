# Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
#
# SPDX-License-Identifier: Apache-2.0
#

set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

find_package(LLVM REQUIRED CONFIG)

add_library(ZLsanPass MODULE zlsan_pass.cpp)


target_compile_definitions(ZLsanPass PRIVATE ${LLVM_DEFINITIONS})
target_include_directories(ZLsanPass PRIVATE ${LLVM_INCLUDE_DIRS})

target_link_options(ZLsanPass PRIVATE
	-Wl,-znodelete
)

zephyr_compile_options(
	"SHELL:-Xclang -load"
	"SHELL:-Xclang ${CMAKE_CURRENT_BINARY_DIR}/libZLsanPass.so"
)

add_custom_target(${ZLSANPASS_TARGET} DEPENDS ZLsanPass)
