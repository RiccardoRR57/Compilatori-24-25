//=============================================================================
// FILE:
//    TestPass.cpp
//
// DESCRIPTION:
//    Visits all functions in a module and prints their names. Strictly speaking,
//    this is an analysis pass (i.e. //    the functions are not modified). However,
//    in order to keep things simple there's no 'print' method here (every analysis
//    pass should implement it).
//
// USAGE:
//    New PM
//      opt -load-pass-plugin=<path-to>libTestPass.so -passes="test-pass" `\`
//        -disable-output <input-llvm-file>
//
//
// License: MIT
//=============================================================================
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.
namespace
{
  // Algebraic Identity pass
  struct AlgIde : PassInfoMixin<AlgIde>
  {
    // Main entry point, takes IR unit to run the pass on (&F) and the
    // corresponding pass manager (to be queried if need be)
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &)
    {
      for (auto B = F.begin(); B != F.end(); ++B)
      {
        for (auto I = (*B).begin(); I != (*B).end(); ++I)
        {
          if (runOnInstruction(*I))
          {
            I = I->eraseFromParent();
            I--;
          }
        }
      }
      return PreservedAnalyses::all();
    }

    bool runOnInstruction(Instruction &I)
    {
      // Mi trovo in una moltiplicazione
      if (MulOperator *mul = dyn_cast<MulOperator>(&I))
      {
        // L'operando 0 è una costante
        if (ConstantInt *c = dyn_cast<ConstantInt>(mul->getOperand(0)))
        {
          // L'operando è un 1
          if (c->getValue() == 1)
          {
            mul->replaceAllUsesWith(mul->getOperand(1));
            // ReplaceInstWithValue(mul, mul->getOperand(1));
            return true;
          }
        }
        // L'operando 1 è una costante
        else if (ConstantInt *c = dyn_cast<ConstantInt>(mul->getOperand(1)))
        {
          // L'operando è un 1
          if (c->getValue() == 1)
          {
            mul->replaceAllUsesWith(mul->getOperand(0));
            return true;
          }
        }
      }
      // Mi trovo in una addizione
      else if (AddOperator *add = dyn_cast<AddOperator>(&I))
      {
        // L'operando 0 è una costante
        if (ConstantInt *c = dyn_cast<ConstantInt>(add->getOperand(0)))
        {
          // L'operando è uno 0
          if (c->getValue() == 0)
          {
            add->replaceAllUsesWith(add->getOperand(1));
            return true;
          }
        }
        // L'operando 1 è una costante
        else if (ConstantInt *c = dyn_cast<ConstantInt>(add->getOperand(1)))
        {
          // L'operando è uno 0
          if (c->getValue() == 0)
          {
            add->replaceAllUsesWith(add->getOperand(0));
            return true;
          }
        }
      }
      return false;
    }
    static bool isRequired() { return true; }
  };

  // Strength Reduction pass
  struct StrRed : PassInfoMixin<StrRed>
  {
    // Main entry point, takes IR unit to run the pass on (&F) and the
    // corresponding pass manager (to be queried if need be)
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &)
    {
      for (auto B = F.begin(); B != F.end(); ++B)
      {
        for (auto I = (*B).begin(); I != (*B).end(); ++I)
        {
          if (runOnInstruction(*I))
          {
            I = I->eraseFromParent();
            I--;
          }
        }
      }
      return PreservedAnalyses::all();
    }

    bool runOnInstruction(Instruction &I)
    {
      errs() << I << '\n';
      if (MulOperator *mul = dyn_cast<MulOperator>(&I))
      {
        if (ConstantInt *c = dyn_cast<ConstantInt>(mul->getOperand(0)))
        {
          if (c->getValue().isPowerOf2())
          {
            Value *shift_val = ConstantInt::get(c->getType(), c->getValue().logBase2());
            Instruction *shift = BinaryOperator::Create(Instruction::Shl, mul->getOperand(1), shift_val);
            shift->insertBefore(&I);
            I.replaceAllUsesWith(shift);
            return true;
          }
        }
        else if (ConstantInt *c = dyn_cast<ConstantInt>(mul->getOperand(1))){
          if (c->getValue().isPowerOf2())
          {
            Value *shift_val = ConstantInt::get(c->getType(), c->getValue().logBase2());
            Instruction *shift = BinaryOperator::Create(Instruction::Shl, mul->getOperand(0), shift_val);
            shift->insertBefore(&I);
            I.replaceAllUsesWith(shift);
            return true;
          }
        }
      }
      else if (BinaryOperator *div = dyn_cast<BinaryOperator>(&I))
      {
        if (div->getOpcode() != Instruction::SDiv && div->getOpcode() != Instruction::UDiv)
          return false;
        else if (ConstantInt *c = dyn_cast<ConstantInt>(div->getOperand(1))) 
        {
          if(c->getValue().isPowerOf2())
          {
            Value *shift_val = ConstantInt::get(c->getType(), c->getValue().logBase2());
            Instruction *shift = BinaryOperator::Create(Instruction::LShr, div->getOperand(0), shift_val);
            shift->insertBefore(&I);
            I.replaceAllUsesWith(shift);
            return true;
          }
        }
      }
      return false;
    }
    static bool isRequired() { return true; }
  };
} // namespace

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getOpts1()
{
  return {LLVM_PLUGIN_API_VERSION, "Assignement1", LLVM_VERSION_STRING,
          [](PassBuilder &PB)
          {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                  if (Name == "algebraic-identity")
                  {
                    FPM.addPass(AlgIde());
                    return true;
                  }
                  else if (Name == "strength-reduction")
                  {
                    FPM.addPass(StrRed());
                    return true;
                  }
                  return false;
                });
          }};
}

// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize TestPass when added to the pass pipeline on the
// command line, i.e. via '-passes=test-pass'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
  return getOpts1();
}
