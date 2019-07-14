/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_LIB_GUI_LVGL_LVGL_COMMON_LVGL_FS_H_
#define ZEPHYR_LIB_GUI_LVGL_LVGL_COMMON_LVGL_FS_H_

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _lv_fs_drv_t;

bool lvgl_fs_ready(struct _lv_fs_drv_t *drv);

lv_fs_res_t errno_to_lv_fs_res(int err);

lv_fs_res_t lvgl_fs_open(struct _lv_fs_drv_t *drv, void *file, const char *path,
		lv_fs_mode_t mode);

lv_fs_res_t lvgl_fs_close(struct _lv_fs_drv_t *drv, void *file);

lv_fs_res_t lvgl_fs_remove(struct _lv_fs_drv_t *drv, const char *path);

lv_fs_res_t lvgl_fs_read(struct _lv_fs_drv_t *drv, void *file, void *buf,
		u32_t btr, u32_t *br);

lv_fs_res_t lvgl_fs_write(struct _lv_fs_drv_t *drv, void *file, const void *buf,
		u32_t btw, u32_t *bw);

lv_fs_res_t lvgl_fs_seek(struct _lv_fs_drv_t *drv, void *file, u32_t pos);

lv_fs_res_t lvgl_fs_tell(struct _lv_fs_drv_t *drv, void *file, u32_t *pos_p);

lv_fs_res_t lvgl_fs_trunc(struct _lv_fs_drv_t *drv, void *file);

lv_fs_res_t lvgl_fs_size(struct _lv_fs_drv_t *drv, void *file, u32_t *fsize);

lv_fs_res_t lvgl_fs_rename(struct _lv_fs_drv_t *drv, const char *from,
		const char *to);

lv_fs_res_t lvgl_fs_free(struct _lv_fs_drv_t *drv, u32_t *total_p,
		u32_t *free_p);

lv_fs_res_t lvgl_fs_dir_open(struct _lv_fs_drv_t *drv, void *dir,
		const char *path);

lv_fs_res_t lvgl_fs_dir_read(struct _lv_fs_drv_t *drv, void *dir,
		char *fn);

lv_fs_res_t lvgl_fs_dir_close(struct _lv_fs_drv_t *drv, void *dir);

void lvgl_fs_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_LIB_GUI_LVGL_LVGL_COMMON_LVGL_FS_H */
