# SPDX-License-Identifier: Apache-2.0


set(ZEPHYR_CURRENT_LIBRARY lvgl)

target_include_directories(lvgl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (CONFIG_LVGL_FILESYSTEM OR CONFIG_LVGL_MEM_POOL_USER OR CONFIG_LVGL_MEM_POOL_USER)

zephyr_library()

zephyr_library_sources_ifdef( CONFIG_LVGL_FILESYSTEM
    lvgl_fs.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_MEM_POOL_USER lvgl_mem_user.c)

zephyr_library_sources_ifdef( CONFIG_LVGL_MEM_POOL_KERNEL lvgl_mem_kernel.c)

zephyr_library_link_libraries(lvgl)

target_link_libraries(lvgl INTERFACE zephyr_interface)

endif()
