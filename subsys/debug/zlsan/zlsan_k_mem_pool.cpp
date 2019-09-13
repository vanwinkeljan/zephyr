/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iostream>

#include "zlsan.hpp"

using namespace std;
using namespace backward;

namespace __zlsan
{

u32_t KernelMemoryPoolManager::k_mem_block_id_to_u32_t(
		const struct k_mem_block_id &id)
{
	return id.pool << 24 | id.level << 20 | id.block;
}

KernelMemoryPoolManager::~KernelMemoryPoolManager()
{
	report();
}

void KernelMemoryPoolManager::init(void)
{
}

void KernelMemoryPoolManager::track(struct k_mem_block *block,
		const size_t size)
{
	auto rval = allocated_mem.emplace(k_mem_block_id_to_u32_t(block->id),
			AllocatedKernelMemoryPoolBlock(block, size));
	if (!rval.second) {
		cout << "## ZLSAN ## - Double allocation detected in "
		     << "kernel memory pool with id " << block->id.pool << " @ "
		     << block->data << " of size " << size << endl;
		StackTrace stack_trace;
		stack_trace.load_here();
		stack_trace.skip_n_firsts(2);
		Printer().print(stack_trace, stdout);
	}
}

bool KernelMemoryPoolManager::untrack(struct k_mem_block_id *id)
{
	auto released_ptr = true;

	if (allocated_mem.erase(k_mem_block_id_to_u32_t(*id)) == 0) {
		cout << "## ZLSAN ## - Double free detected" << endl;
		StackTrace stack_trace;
		stack_trace.load_here();
		stack_trace.skip_n_firsts(2);
		Printer().print(stack_trace, stdout);
		released_ptr = false;
	}

	return released_ptr;
}

void KernelMemoryPoolManager::report() const
{
	if (!hasMemoryAllocated()) {
		return;
	}

	for (auto &key_val : allocated_mem) {
		auto &val = key_val.second;
		cout << "## ZLSAN ## - Resources leaked in kernel memory pool "
		     << "with id " << val.getPoolId() << ": " << val.getSize()
		     << " bytes leaked @ " << val.getBaseAddress() << endl;
		Printer printer;
		printer.print(val.getStackTrace(), stdout);
	}
}

bool KernelMemoryPoolManager::hasMemoryAllocated(void) const
{
	return !allocated_mem.empty();
}

AllocatedKernelMemoryPoolBlock::AllocatedKernelMemoryPoolBlock(
	const struct k_mem_block *block, const size_t size)
	: block(*block), size(size), stack_trace()
{
	stack_trace.load_here();
	stack_trace.skip_n_firsts(2);
}

const void *AllocatedKernelMemoryPoolBlock::getBaseAddress(void) const
{
	return block.data;
}

size_t AllocatedKernelMemoryPoolBlock::getSize(void) const
{
	return size;
}

u32_t AllocatedKernelMemoryPoolBlock::getPoolId(void) const
{
	return block.id.pool;
}

const StackTrace &AllocatedKernelMemoryPoolBlock::getStackTrace(void) const
{
	return stack_trace;
}

} // namespace __zlsan

extern "C" int __zslan_k_mem_pool_alloc(struct k_mem_pool *pool,
		struct k_mem_block *block, size_t size, s32_t timeout)
{
	auto rval = k_mem_pool_alloc(pool, block, size, timeout);
	if (rval == 0) {
		__zlsan::k_mem_pool_manager->track(block, size);
	}
	return rval;
}

extern "C" void __zlsan_k_mem_pool_free_id(struct k_mem_block_id *id)
{
	auto released_ptr = __zlsan::k_mem_pool_manager->untrack(id);
	if (released_ptr) {
		k_mem_pool_free_id(id);
	}
}
