/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string>
#include <vector>
#include <utility>

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/CallSite.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

using namespace llvm;
using namespace std;

namespace
{
class ZLsanPass : public ModulePass {
public:
	static char ID;
	ZLsanPass();

	bool runOnModule(Module &M) override;

private:
	bool wrapFunction(Module &M, const string &f_name,
			  const string &w_name);

	vector<pair<string, string> > FunctionsToWrap = {
		make_pair("k_mem_pool_alloc", "__zslan_k_mem_pool_alloc"),
		make_pair("k_mem_pool_free_id", "__zlsan_k_mem_pool_free_id"),
	};
};

char ZLsanPass::ID = 0;
ZLsanPass::ZLsanPass() : ModulePass(ID)
{
}

bool ZLsanPass::runOnModule(Module &M)
{
	auto changed = false;

	for (auto &func_to_wrap : FunctionsToWrap) {
		changed |= wrapFunction(M, func_to_wrap.first,
					func_to_wrap.second);
	}

	return changed;
}

bool ZLsanPass::wrapFunction(Module &M, const string &f_name,
			     const string &w_name)
{
	auto changed = false;
	auto F = M.getFunction(f_name);

	if (F == nullptr) {
		return false;
	}

	auto wF = M.getOrInsertFunction(w_name, F->getFunctionType());

	for (auto *user : F->users()) {
		Instruction *old_call = nullptr;
		if ((old_call = dyn_cast<Instruction>(user)) == nullptr) {
			continue;
		}

		CallSite call_site(old_call);
		if (!call_site) {
			continue;
		}

		/* Make sure the caller is not the wrapper function it self */
		if (call_site.getParent()->getParent() == wF) {
			continue;
		}

		Instruction *new_inst = nullptr;
		vector<Value *> args(call_site.arg_begin(),
				     call_site.arg_end());
		if (InvokeInst *invoke = dyn_cast<InvokeInst>(old_call)) {
			auto new_invoke =
				InvokeInst::Create(wF, invoke->getNormalDest(),
						   invoke->getUnwindDest(),
						   args, "", old_call);
			new_invoke->setCallingConv(call_site.getCallingConv());
			new_inst = cast<Instruction>(new_invoke);
		} else {
			auto new_call = CallInst::Create(wF, args, "", old_call);
			new_call->setCallingConv(call_site.getCallingConv());
			new_inst = cast<Instruction>(new_call);
		}
		new_inst->setDebugLoc(old_call->getDebugLoc());

		if (!old_call->use_empty()) {
			old_call->replaceAllUsesWith(new_inst);
		}

		new_inst->takeName(old_call);
		old_call->eraseFromParent();
	}

	return changed;
}

} /* namespace */

static RegisterPass<ZLsanPass> ZLSPI("ZLsanPass",
				     "Zephyr Memory Leak Sanitizer Pass",
				     false, false);

static RegisterStandardPasses ZLSP(PassManagerBuilder::EP_ModuleOptimizerEarly,
				   [](const PassManagerBuilder &Builder,
				      legacy::PassManagerBase &PM) {
					   PM.add(new ZLsanPass());
				   });

static RegisterStandardPasses ZLSP0(PassManagerBuilder::EP_EnabledOnOptLevel0,
				    [](const PassManagerBuilder &Builder,
				       legacy::PassManagerBase &PM) {
					    PM.add(new ZLsanPass());
				    });
