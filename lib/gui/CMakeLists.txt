# SPDX-License-Identifier: Apache-2.0

if (CONFIG_LVGL)
add_subdirectory_if_kconfig(lvgl_v5)
add_subdirectory_if_kconfig(lvgl_v6)
add_subdirectory(lvgl_common)
endif()
