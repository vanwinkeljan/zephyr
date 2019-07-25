/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <lvgl.h>
#include <zephyr.h>
#include <fs/fs.h>

bool lvgl_fs_ready(struct _lv_fs_drv_t *drv)
{
	return true;
}

lv_fs_res_t errno_to_lv_fs_res(int err)
{
	switch (err) {
	case 0:
		return LV_FS_RES_OK;
	case -EIO:
		/*Low level hardware error*/
		return LV_FS_RES_HW_ERR;
	case -EBADF:
		/*Error in the file system structure */
		return LV_FS_RES_FS_ERR;
	case -ENOENT:
		/*Driver, file or directory is not exists*/
		return LV_FS_RES_NOT_EX;
	case -EFBIG:
		/*Disk full*/
		return LV_FS_RES_FULL;
	case -EACCES:
		/*Access denied. Check 'fs_open' modes and write protect*/
		return LV_FS_RES_DENIED;
	case -EBUSY:
		/*The file system now can't handle it, try later*/
		return LV_FS_RES_BUSY;
	case -ENOMEM:
		/*Not enough memory for an internal operation*/
		return LV_FS_RES_OUT_OF_MEM;
	case -EINVAL:
		/*Invalid parameter among arguments*/
		return LV_FS_RES_INV_PARAM;
	default:
		return LV_FS_RES_UNKNOWN;
	}
}

lv_fs_res_t lvgl_fs_open(struct _lv_fs_drv_t *drv, void *file,
		const char *path, lv_fs_mode_t mode)
{
	int err;

	err = fs_open((struct fs_file_t *)file, path);
	return errno_to_lv_fs_res(err);
}

lv_fs_res_t lvgl_fs_close(struct _lv_fs_drv_t *drv, void *file)
{
	int err;

	err = fs_close((struct fs_file_t *)file);
	return errno_to_lv_fs_res(err);
}

lv_fs_res_t lvgl_fs_remove(struct _lv_fs_drv_t *drv, const char *path)
{
	int err;

	err = fs_unlink(path);
	return errno_to_lv_fs_res(err);
}

lv_fs_res_t lvgl_fs_read(struct _lv_fs_drv_t *drv, void *file,
		void *buf, u32_t btr, u32_t *br)
{
	int err;

	err = fs_read((struct fs_file_t *)file, buf, btr);
	if (err > 0) {
		if (br != NULL) {
			*br = err;
		}
		err = 0;
	} else if (br != NULL) {
		*br = 0U;
	}
	return errno_to_lv_fs_res(err);
}

lv_fs_res_t lvgl_fs_write(struct _lv_fs_drv_t *drv, void *file,
		const void *buf, u32_t btw, u32_t *bw)
{
	int err;

	err = fs_write((struct fs_file_t *)file, buf, btw);
	if (err == btw) {
		if (bw != NULL) {
			*bw = btw;
		}
		err = 0;
	} else if (err < 0) {
		if (bw != NULL) {
			*bw = 0U;
		}
	} else {
		if (bw != NULL) {
			*bw = err;
		}
		err = -EFBIG;
	}
	return errno_to_lv_fs_res(err);
}

lv_fs_res_t lvgl_fs_seek(struct _lv_fs_drv_t *drv, void *file, u32_t pos)
{
	int err;

	err = fs_seek((struct fs_file_t *)file, pos, FS_SEEK_SET);
	return errno_to_lv_fs_res(err);
}

lv_fs_res_t lvgl_fs_tell(struct _lv_fs_drv_t *drv, void *file,
		u32_t *pos_p)
{
	*pos_p = fs_tell((struct fs_file_t *)file);
	return LV_FS_RES_OK;
}

lv_fs_res_t lvgl_fs_trunc(struct _lv_fs_drv_t *drv, void *file)
{
	int err;
	off_t length;

	length = fs_tell((struct fs_file_t *) file);
	++length;
	err = fs_truncate((struct fs_file_t *)file, length);
	return errno_to_lv_fs_res(err);
}

lv_fs_res_t lvgl_fs_size(struct _lv_fs_drv_t *drv, void *file,
		u32_t *fsize)
{
	int err;
	off_t org_pos;

	/* LVGL does not provided path but pointer to file struct as such
	 * we can not use fs_stat, instead use a combination of fs_tell and
	 * fs_seek to get the files size.
	 */

	org_pos = fs_tell((struct fs_file_t *) file);

	err = fs_seek((struct fs_file_t *) file, 0, FS_SEEK_END);
	if (err != 0) {
		*fsize = 0U;
		return errno_to_lv_fs_res(err);
	}

	*fsize = fs_tell((struct fs_file_t *) file) + 1;

	err = fs_seek((struct fs_file_t *) file, org_pos, FS_SEEK_SET);

	return errno_to_lv_fs_res(err);
}

lv_fs_res_t lvgl_fs_rename(struct _lv_fs_drv_t *drv, const char *from,
		const char *to)
{
	int err;

	err = fs_rename(from, to);
	return errno_to_lv_fs_res(err);
}

lv_fs_res_t lvgl_fs_free(struct _lv_fs_drv_t *drv, u32_t *total_p,
		u32_t *free_p)
{
	/* We have no easy way of telling the total file system size.
	 * Zephyr can only return this information per mount point.
	 */
	return LV_FS_RES_NOT_IMP;
}

lv_fs_res_t lvgl_fs_dir_open(struct _lv_fs_drv_t *drv, void *dir,
		const char *path)
{
	int err;

	err = fs_opendir((struct fs_dir_t *)dir, path);
	return errno_to_lv_fs_res(err);
}

lv_fs_res_t lvgl_fs_dir_read(struct _lv_fs_drv_t *drv, void *dir,
		char *fn)
{
	/* LVGL expects a string as return parameter but the format of the
	 * string is not documented.
	 */
	return LV_FS_RES_NOT_IMP;
}

lv_fs_res_t lvgl_fs_dir_close(struct _lv_fs_drv_t *drv, void *dir)
{
	int err;

	err = fs_closedir((struct fs_dir_t *)dir);
	return errno_to_lv_fs_res(err);
}
