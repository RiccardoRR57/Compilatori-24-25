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
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"

using namespace llvm;

//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.
namespace
{



  // Helper function to check if an instruction is in a vector of instructions
  bool contains(llvm::Instruction* I, std::vector<llvm::Instruction*> &V) {
    for(auto &inst : V) {
      if(I == inst) {
        return true;
      }
    }
    return false;
  }
  
  // New PM implementation
  struct LoopInvariant : PassInfoMixin<LoopInvariant>
  {
    // Main entry point, takes IR unit to run the pass on (&F) and the
    // corresponding pass manager (to be queried if need be)
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM)
    {
      LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
      DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);

      for(auto *L : LI) {
        runOnLoop(*L, DT);
      }

      return PreservedAnalyses::all();
    }

    void runOnLoop(Loop &L, DominatorTree &DT) {

      std::vector<llvm::Instruction*> loopInv = getLoopInvInstr(L);

      codeMotion(L, loopInv, DT);

      // Print the loop invariant instructions
      errs() << "Loop invariant instructions in loop: \n";
      for(auto &I : loopInv) {
        errs() << *I << "\n";
      }
      errs() << "\n";
      for(Loop::block_iterator BI = L.block_begin(); BI != L.block_end(); ++BI) {
        llvm::BasicBlock *BB = *BI;
        for(auto &I : *BB) {
          if(L.isLoopInvariant(&I)) {
            errs() << "Loop invariant instruction: " << I << "\n";
          }
        }
      }
    }

    std::vector<Instruction*> getLoopInvInstr(Loop &L) {
      // LoopInvInst is the vector of loop invariant instructions
      std::vector<Instruction*> LoopInvInst = {};
      // TODO ragionare se l'istruzione è una phi function cosa sta succedendo

      for(Loop::block_iterator BI = L.block_begin(); BI != L.block_end(); ++BI) {
        llvm::BasicBlock *BB = *BI;
        for(auto &I : *BB) {
          bool isLoopInvariant = true;
          for(auto &op : I.operands()) {
            // Check if the operator is a constant
            if(Instruction* inst = dyn_cast<Instruction>(op)) {
              if(L.contains(inst->getParent()) && !contains(inst, LoopInvInst) ) {
                // TODO qui la funzione contains non è sufficiente, potrebbe non essere nella nostra lista perchè non è stata ancora anlizzata
                isLoopInvariant = false;
                break;
              }
            }
          }
          if(isLoopInvariant) {
            LoopInvInst.push_back(&I);
          }
        }
      }
      return LoopInvInst;
    } 

    void codeMotion(Loop &L, std::vector<Instruction*> &loopInv, DominatorTree &DT) {
      for(auto &I : loopInv) {
        bool candidate = true;
        llvm::SmallVector<BasicBlock*> ExitBlocks;
        L.getExitBlocks(ExitBlocks);
        for(auto &Exit : ExitBlocks) {
          if(!DT.dominates(I->getParent(), Exit) && !isInstrDead(I, Exit)) {
            candidate = false;
            break;
          }
        }
        // TODO spostare le itruzioni :: cazzi nostri
      }
    }

    bool isInstrDead(Instruction* I, BasicBlock* ExitBlock) {
      // TODO metodi di basicblock per scorrere i suoi successori
      return true;
    }
    
    // Without isRequired returning true, this pass will be skipped for functions
    // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
    // all functions with optnone.s
    static bool isRequired() { return true; }
  };  
} // namespace

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getTestPassPluginInfo()
{
  return {LLVM_PLUGIN_API_VERSION, "LoopInvariant", LLVM_VERSION_STRING,
          [](PassBuilder &PB)
          {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                  if (Name == "loop-invariant")
                  {
                    FPM.addPass(LoopInvariant());
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
  return getTestPassPluginInfo();
}
