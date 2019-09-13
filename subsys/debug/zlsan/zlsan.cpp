/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "zlsan.hpp"

namespace __zlsan
{

KernelMemoryPoolManager *k_mem_pool_manager;


void init(void)
{
	k_mem_pool_manager = new KernelMemoryPoolManager;
}

void exit_check(void)
{
	int exit_code = k_mem_pool_manager->hasMemoryAllocated() ?
		CONFIG_ZLSAN_EXIT_CODE : 0;
	delete k_mem_pool_manager;

	if (exit_code != 0) {
		_exit(exit_code);
	}
}


} // namespace __zlsan

/* Make sure that __zlsan::init runs before anything else by placing it in the
 * .preinit_array section
 */
__attribute__((section(".preinit_array"),
	       used)) void (*__zlsan_preinit)(void) = ::__zlsan::init;

/* Run final check at exit to detect any leaks */
__attribute__((section(".fini_array"),
	       used)) void (*__zlsan_fini)(void) = ::__zlsan::exit_check;
