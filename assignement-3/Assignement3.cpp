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
  // New PM implementation
  struct LoopInvariant : PassInfoMixin<LoopInvariant>
  {
    bool isLoopInv(BinaryOperator &I, Loop &L) {
      Value* firstOp = I.getOperand(0);
      Value* secondOp = I.getOperand(1);
      
      bool firstOpLI = false;
      bool secondOpLI = false;

      if(dyn_cast<Constant>(firstOp)) firstOpLI = true;
      if(Instruction* inst = dyn_cast<Instruction>(firstOp)) {
        if(!L.contains(inst->getParent())) firstOpLI = true;
        if(BinaryOperator* binOp = dyn_cast<BinaryOperator>(inst)) firstOpLI = isLoopInv(*binOp, L);
      }
      
      if(dyn_cast<Constant>(secondOp)) secondOpLI = true;
      if(Instruction* inst = dyn_cast<Instruction>(secondOp)) {
        if(!L.contains(inst->getParent())) secondOpLI = true;
        if(BinaryOperator* binOp = dyn_cast<BinaryOperator>(inst)) secondOpLI = isLoopInv(*binOp, L);
      }

      return firstOpLI && secondOpLI;
    }

    std::vector<Instruction*> getLoopInvInstr(Loop &L) {
      // LoopInvInst is the vector of loop invariant instructions
      std::vector<Instruction*> LoopInvInst = {};
      // TODO ragionare se l'istruzione è una phi function cosa sta succedendo
      // L'istruzione per controllare se un'istruzione è una phi function è: 
      //llvm::<Instruction*> I;
      //isa<PHINode>(I) 

      for(Loop::block_iterator BI = L.block_begin(); BI != L.block_end(); ++BI) {
        llvm::BasicBlock *BB = *BI;
        for(auto &I : *BB) {
          if(BinaryOperator *binOp = dyn_cast<BinaryOperator>(&I)) {
            if(isLoopInv(*binOp, L)) {
              LoopInvInst.push_back(&I);
            }
          }
        }
      }
      return LoopInvInst;
    } 


    bool isInstrDead(Instruction* I, BasicBlock* ExitBlock) {
      // Scorro su i successori del blocco d'uscita
      for (BasicBlock *Succ : successors(ExitBlock)) {
        for (User* U : I->users()) {
          // Casto l'uso a un'istruzione
          if (Instruction* UseInst = dyn_cast<Instruction>(U)) {
            // Se hanno la stessa reaching definition allora sono la stessa istruzione, dunque non è morta.
            if (UseInst->getParent() == Succ) {
                return false;
            }
          }
        }
      }
      return true;
    }

    void codeMotion(Loop &L, std::vector<Instruction*> &loopInv, DominatorTree &DT) {
      BasicBlock *PH = L.getLoopPreheader();

      for(auto &I : loopInv) {
        outs()<< "Sto analizzando l'istruzione: " << *I << '\n';
        bool candidate = true;
        llvm::SmallVector<BasicBlock*> ExitBlocks;
        L.getExitBlocks(ExitBlocks);
        for(auto &Exit : ExitBlocks) {
          if(!DT.dominates(I->getParent(), Exit)) outs() << "L'istruzione non domina l'uscita del loop\n";
          if(!isInstrDead(I,Exit)) outs() << "L'istruzione non è dead\n";
          if(!DT.dominates(I->getParent(), Exit) && !isInstrDead(I, Exit)) {
            candidate = false;
            break;
          }
        }

        if(candidate) {
          I->moveBefore(PH->getTerminator());
          outs() << "Moved instruction: " << *I << "\n";
        }
      }
    }

    void runOnLoop(Loop &L, DominatorTree &DT) {

      std::vector<llvm::Instruction*> loopInv = getLoopInvInstr(L);

      codeMotion(L, loopInv, DT);

      // Print the loop invariant instructions
      errs() << "Loop invariant instructions in loop: \n";
      for(auto &I : loopInv) {
        errs() << *I << "\n";
      }
    }

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
