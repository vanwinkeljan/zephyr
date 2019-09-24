/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "kernel.h"
#include <ztest.h>

void test_kernel_heap_leak(void)
{
	void *ptr = k_malloc(10);

	zassert_not_null(ptr, NULL);
}

void test_kernel_heap_double_free(void)
{
	void *ptr = k_malloc(10);

	zassert_not_null(ptr, NULL);
	k_free(ptr);
	k_free(ptr);
}

void test_main(void)
{
	ztest_test_suite(kernel_heap_tests,
			 ztest_unit_test(test_kernel_heap_leak),
			 ztest_unit_test(test_kernel_heap_double_free));
	ztest_run_test_suite(kernel_heap_tests);
}
