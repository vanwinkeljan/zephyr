/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <lvgl.h>
#include <zephyr.h>
#include <fs/fs.h>
#include "lvgl_fs.h"

/* Stub for LVGL ufs (ram based FS) init function
 * as we use Zephyr FS API instead
 */
void lv_ufs_init(void)
{
}

static bool lvgl_v5_fs_ready(void)
{
	return lvgl_fs_ready(NULL);
}

static lv_fs_res_t lvgl_v5_fs_open(void *file, const char *path,
		lv_fs_mode_t mode)
{
	return lvgl_fs_open(NULL, file, path, mode);
}

static lv_fs_res_t lvgl_v5_fs_close(void *file)
{
	return lvgl_fs_close(NULL, file);
}

static lv_fs_res_t lvgl_v5_fs_remove(const char *path)
{
	return lvgl_fs_remove(NULL, path);
}

static lv_fs_res_t lvgl_v5_fs_read(void *file, void *buf, u32_t btr,
		u32_t *br)
{
	return lvgl_fs_read(NULL, file, buf, btr, br);
}

static lv_fs_res_t lvgl_v5_fs_write(void *file, const void *buf, u32_t btw,
		u32_t *bw)
{
	return lvgl_fs_write(NULL, file, buf, btw, bw);
}

static lv_fs_res_t lvgl_v5_fs_seek(void *file, u32_t pos)
{
	return lvgl_fs_seek(NULL, file, pos);
}

static lv_fs_res_t lvgl_v5_fs_tell(void *file, u32_t *pos_p)
{
	return lvgl_fs_tell(NULL, file, pos_p);
}

static lv_fs_res_t lvgl_v5_fs_trunc(void *file)
{
	return lvgl_fs_trunc(NULL, file);
}

static lv_fs_res_t lvgl_v5_fs_size(void *file, u32_t *fsize)
{
	return lvgl_fs_size(NULL, file, fsize);
}

static lv_fs_res_t lvgl_v5_fs_rename(const char *from, const char *to)
{
	return lvgl_fs_rename(NULL, from, to);
}

static lv_fs_res_t lvgl_v5_fs_free(u32_t *total_p, u32_t *free_p)
{
	return lvgl_fs_free(NULL, total_p, free_p);
}

static lv_fs_res_t lvgl_v5_fs_dir_open(void *dir, const char *path)
{
	return lvgl_fs_dir_open(NULL, dir, path);
}

static lv_fs_res_t lvgl_v5_fs_dir_read(void *dir, char *fn)
{
	return lvgl_fs_dir_read(NULL, dir, fn);
}

static lv_fs_res_t lvgl_v5_fs_dir_close(void *dir)
{
	return lvgl_fs_dir_close(NULL, dir);
}


void lvgl_fs_init(void)
{
	lv_fs_drv_t fs_drv;

	memset(&fs_drv, 0, sizeof(lv_fs_drv_t));

	fs_drv.file_size = sizeof(struct fs_file_t);
	fs_drv.rddir_size = sizeof(struct fs_dir_t);
	fs_drv.letter = '/';
	fs_drv.ready = lvgl_v5_fs_ready;

	fs_drv.open = lvgl_v5_fs_open;
	fs_drv.close = lvgl_v5_fs_close;
	fs_drv.remove = lvgl_v5_fs_remove;
	fs_drv.read = lvgl_v5_fs_read;
	fs_drv.write = lvgl_v5_fs_write;
	fs_drv.seek = lvgl_v5_fs_seek;
	fs_drv.tell = lvgl_v5_fs_tell;
	fs_drv.trunc = lvgl_v5_fs_trunc;
	fs_drv.size = lvgl_v5_fs_size;
	fs_drv.rename = lvgl_v5_fs_rename;
	fs_drv.free = lvgl_v5_fs_free;

	fs_drv.dir_open = lvgl_v5_fs_dir_open;
	fs_drv.dir_read = lvgl_v5_fs_dir_read;
	fs_drv.dir_close = lvgl_v5_fs_dir_close;

	lv_fs_add_drv(&fs_drv);
}

