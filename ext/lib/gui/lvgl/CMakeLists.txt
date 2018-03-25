
zephyr_interface_library_named(lvgl)

target_include_directories(lvgl INTERFACE ${CONFIG_LVGL_SOURCE_DIR})
target_include_directories(lvgl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

zephyr_compile_definitions(LV_CONF_INCLUDE_SIMPLE=1)

zephyr_library()

zephyr_library_sources(

    ${CONFIG_LVGL_SOURCE_DIR}/lv_core/lv_group.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_core/lv_indev.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_core/lv_obj.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_core/lv_refr.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_core/lv_style.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_core/lv_vdb.c

    ${CONFIG_LVGL_SOURCE_DIR}/lv_draw/lv_draw.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_draw/lv_draw_arc.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_draw/lv_draw_img.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_draw/lv_draw_label.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_draw/lv_draw_line.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_draw/lv_draw_rbasic.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_draw/lv_draw_rect.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_draw/lv_draw_triangle.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_draw/lv_draw_vbasic.c

    ${CONFIG_LVGL_SOURCE_DIR}/lv_hal/lv_hal_disp.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_hal/lv_hal_indev.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_hal/lv_hal_tick.c

    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_anim.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_area.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_circ.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_color.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_font.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_fs.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_ll.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_log.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_math.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_mem.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_task.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_templ.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_misc/lv_txt.c

    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_arc.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_bar.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_btn.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_btnm.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_calendar.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_cb.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_chart.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_cont.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_ddlist.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_gauge.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_img.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_imgbtn.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_kb.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_label.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_led.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_line.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_list.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_lmeter.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_mbox.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_objx_templ.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_page.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_preload.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_roller.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_slider.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_sw.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_ta.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_tabview.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_objx/lv_win.c

    ${CONFIG_LVGL_SOURCE_DIR}/lv_themes/lv_theme.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_themes/lv_theme_alien.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_themes/lv_theme_default.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_themes/lv_theme_material.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_themes/lv_theme_mono.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_themes/lv_theme_nemo.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_themes/lv_theme_night.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_themes/lv_theme_templ.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_themes/lv_theme_zen.c

    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_builtin.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_BUILD_IN_FONT_10
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_10.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_10_cyrillic.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_10_latin_sup.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_symbol_10.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_BUILD_IN_FONT_20
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_20.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_20_cyrillic.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_20_latin_sup.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_symbol_20.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_BUILD_IN_FONT_30
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_30.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_30_cyrillic.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_30_latin_sup.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_symbol_30.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_BUILD_IN_FONT_40
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_40.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_40_cyrillic.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_dejavu_40_latin_sup.c
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_symbol_40.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_BUILD_IN_FONT_MONOSPACE
    ${CONFIG_LVGL_SOURCE_DIR}/lv_fonts/lv_font_monospace_8.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_COLOR_DEPTH_32
    lvgl_color_32.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_COLOR_DEPTH_16
    lvgl_color_16.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_COLOR_DEPTH_8
    lvgl_color_8.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_COLOR_DEPTH_1
    lvgl_color_1.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_FILESYSTEM
    lvgl_fs.c
)

zephyr_library_sources(lvgl.c)

zephyr_library_sources_ifdef( CONFIG_LVGL_MEM_POOL_USER lvgl_mem_user.c)

zephyr_library_sources_ifdef( CONFIG_LVGL_MEM_POOL_KERNEL lvgl_mem_kernel.c)

zephyr_library_link_libraries(lvgl)

target_link_libraries(lvgl INTERFACE zephyr_interface)
