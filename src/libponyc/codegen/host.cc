#include "llvm_config_begin.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>

#include "llvm_config_end.h"

#include <stdio.h>
#include "codegen.h"
#include "genopt.h"
#include "ponyassert.h"

using namespace llvm;

LLVMTargetMachineRef codegen_machine(LLVMTargetRef target, pass_opt_t* opt)
{
  Optional<Reloc::Model> reloc;

  if(opt->pic || opt->library)
    reloc = Reloc::PIC_;

  // The Arm debug fix is a "temporary" fix for issue #3874
  // https://github.com/ponylang/ponyc/issues/3874
  // Hopefully we get #3874 figured out in a reasonable amount of time.
  CodeGenOpt::Level opt_level =
    opt->release ? CodeGenOpt::Aggressive :
      target_is_arm(opt->triple) ? CodeGenOpt::Default : CodeGenOpt::None;

  TargetOptions options;
  options.TrapUnreachable = true;

  Target* t = reinterpret_cast<Target*>(target);

TargetMachine* m = t->createTargetMachine(opt->triple, opt->cpu,
  opt->features, options, reloc, llvm::None, opt_level, false);

  return reinterpret_cast<LLVMTargetMachineRef>(m);
}

void LLVMSetUnsafeAlgebra(LLVMValueRef inst)
{
  unwrap<Instruction>(inst)->setHasAllowReassoc(true);
}

void LLVMSetNoUnsignedWrap(LLVMValueRef inst)
{
  unwrap<BinaryOperator>(inst)->setHasNoUnsignedWrap(true);
}

void LLVMSetNoSignedWrap(LLVMValueRef inst)
{
  unwrap<BinaryOperator>(inst)->setHasNoSignedWrap(true);
}

void LLVMSetIsExact(LLVMValueRef inst)
{
  unwrap<BinaryOperator>(inst)->setIsExact(true);
}

LLVMValueRef LLVMConstNaN(LLVMTypeRef type)
{
  Type* t = unwrap<Type>(type);

  Value* nan = ConstantFP::getNaN(t);
  return wrap(nan);
}

LLVMValueRef LLVMConstInf(LLVMTypeRef type, bool negative)
{
  Type* t = unwrap<Type>(type);

  Value* inf = ConstantFP::getInfinity(t, negative);
  return wrap(inf);
}

LLVMModuleRef LLVMParseIRFileInContext(LLVMContextRef ctx, const char* file)
{
  SMDiagnostic diag;

  return wrap(parseIRFile(file, diag, *unwrap(ctx)).release());
}

// From LLVM internals.
static MDNode* extractMDNode(MetadataAsValue* mdv)
{
  Metadata* md = mdv->getMetadata();
  pony_assert(isa<MDNode>(md) || isa<ConstantAsMetadata>(md));

  MDNode* n = dyn_cast<MDNode>(md);
  if(n != NULL)
    return n;

  return MDNode::get(mdv->getContext(), md);
}

bool LLVMHasMetadataStr(LLVMValueRef val, const char* str)
{
  Value* v = unwrap<Value>(val);
  Instruction* i = dyn_cast<Instruction>(v);

  if(i != NULL)
    return i->getMetadata(str) != NULL;

  return cast<Function>(v)->getMetadata(str) != NULL;
}

void LLVMSetMetadataStr(LLVMValueRef val, const char* str, LLVMValueRef node)
{
  pony_assert(node != NULL);

  MDNode* n = extractMDNode(unwrap<MetadataAsValue>(node));
  Value* v = unwrap<Value>(val);
  Instruction* i = dyn_cast<Instruction>(v);

  if(i != NULL)
    i->setMetadata(str, n);
  else
    cast<Function>(v)->setMetadata(str, n);
}

void LLVMMDNodeReplaceOperand(LLVMValueRef parent, unsigned i,
  LLVMValueRef node)
{
  pony_assert(parent != NULL);
  pony_assert(node != NULL);

  MDNode *pn = extractMDNode(unwrap<MetadataAsValue>(parent));
  MDNode *cn = extractMDNode(unwrap<MetadataAsValue>(node));
  pn->replaceOperandWith(i, cn);
}

LLVMValueRef LLVMMemcpy(LLVMModuleRef module, bool ilp32)
{
  Module* m = unwrap(module);

  Type* params[3];
  params[0] = Type::getInt8PtrTy(m->getContext());
  params[1] = params[0];
  params[2] = Type::getIntNTy(m->getContext(), ilp32 ? 32 : 64);

  return wrap(Intrinsic::getDeclaration(m, Intrinsic::memcpy, {params, 3}));
}

LLVMValueRef LLVMMemmove(LLVMModuleRef module, bool ilp32)
{
  Module* m = unwrap(module);

  Type* params[3];
  params[0] = Type::getInt8PtrTy(m->getContext());
  params[1] = params[0];
  params[2] = Type::getIntNTy(m->getContext(), ilp32 ? 32 : 64);

  return wrap(Intrinsic::getDeclaration(m, Intrinsic::memmove, {params, 3}));
}

LLVMValueRef LLVMLifetimeStart(LLVMModuleRef module, LLVMTypeRef type)
{
  Module* m = unwrap(module);

  Type* t[1] = { unwrap(type) };
  return wrap(Intrinsic::getDeclaration(m, Intrinsic::lifetime_start, t));
}

LLVMValueRef LLVMLifetimeEnd(LLVMModuleRef module, LLVMTypeRef type)
{
  Module* m = unwrap(module);

  Type* t[1] = { unwrap(type) };
  return wrap(Intrinsic::getDeclaration(m, Intrinsic::lifetime_end, t));
}
