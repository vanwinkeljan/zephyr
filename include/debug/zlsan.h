/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DEBUG_ZLSAN_H_
#define ZEPHYR_INCLUDE_DEBUG_ZLSAN_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print a report with all potential memory leaks
 *
 */
void zlsan_report_all_leaks(void);

/**
 * @brief Check if any memory was leaked
 *
 * @retval True if memory is leaked
 */
bool zlsan_detected_memory_leak(void);

/**
 * @brief Print a report with all potential memory leaks in a kernel memory pool
 *
 * @param pool_id ID of the kernel memory pool or -1 for all kernel memory pools
 */
void zlsan_report_kernel_memory_pool_leaks(int pool_id);

/**
 * @brief Check if any memory was leaked from a kernel memory pool
 *
 * @param pool_id ID of the kernel memory pool or -1 for all kernel memory pools
 *
 * @retval True if memory is leaked
 */
bool zlsan_detected_kernel_memory_pool_leak(int pool_id);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_DEBUG_ZLSAN_H_ */
