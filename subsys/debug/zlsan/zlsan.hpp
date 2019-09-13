/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SUBSYS_DEBUG_ZLSAN_ZLSAN_HPP__
#define __SUBSYS_DEBUG_ZLSAN_ZLSAN_HPP__

#include <stddef.h>
#include <unordered_map>
#include <kernel.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <backward.hpp>

namespace __zlsan
{

class AllocatedKernelMemoryPoolBlock {
public:
	AllocatedKernelMemoryPoolBlock(const struct k_mem_block *block,
					const size_t size);

	const void *getBaseAddress(void) const;
	size_t getSize(void) const;
	u32_t getPoolId(void) const;
	const backward::StackTrace &getStackTrace(void) const;

private:
	struct k_mem_block block;
	size_t size;
	backward::StackTrace stack_trace;
};

class KernelMemoryPoolManager {
public:
	~KernelMemoryPoolManager();

	void init(void);
	void track(struct k_mem_block *block, const size_t size);
	bool untrack(struct k_mem_block_id *id);

	void report(void) const;
	bool hasMemoryAllocated(void) const;

private:
	static u32_t k_mem_block_id_to_u32_t(const struct k_mem_block_id &id);
	std::unordered_map<u32_t, AllocatedKernelMemoryPoolBlock>
		allocated_mem;
};

extern KernelMemoryPoolManager *k_mem_pool_manager;

} /* namespace __zlsan*/

#endif /* __SUBSYS_DEBUG_ZLSAN_ZLSAN_HPP__ */
