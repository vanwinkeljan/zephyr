/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <lvgl.h>
#include <zephyr.h>
#include <fs/fs.h>
#include "lvgl_fs.h"

void lvgl_fs_init(void)
{
	lv_fs_drv_t fs_drv;
	lv_fs_drv_init(&fs_drv);

	fs_drv.file_size = sizeof(struct fs_file_t);
	fs_drv.rddir_size = sizeof(struct fs_dir_t);
	fs_drv.letter = '/';
	fs_drv.ready_cb = lvgl_fs_ready;

	fs_drv.open_cb = lvgl_fs_open;
	fs_drv.close_cb = lvgl_fs_close;
	fs_drv.remove_cb = lvgl_fs_remove;
	fs_drv.read_cb = lvgl_fs_read;
	fs_drv.write_cb = lvgl_fs_write;
	fs_drv.seek_cb = lvgl_fs_seek;
	fs_drv.tell_cb = lvgl_fs_tell;
	fs_drv.trunc_cb = lvgl_fs_trunc;
	fs_drv.size_cb = lvgl_fs_size;
	fs_drv.rename_cb = lvgl_fs_rename;
	fs_drv.free_space_cb = lvgl_fs_free;

	fs_drv.dir_open_cb = lvgl_fs_dir_open;
	fs_drv.dir_read_cb = lvgl_fs_dir_read;
	fs_drv.dir_close_cb = lvgl_fs_dir_close;

	lv_fs_drv_register(&fs_drv);
}

