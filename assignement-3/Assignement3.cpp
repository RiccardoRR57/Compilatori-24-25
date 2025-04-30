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

    bool isAnalized(llvm::Instruction* I, std::vector<llvm::Instruction*> &V) {
      for(auto &inst : V) {
        if(I == inst) {
          return true;
        }
      }
      return false;
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
      // L'istruzione per controllare se un'istruzione è una phi function è: 
      llvm::<Instruction*> I;
      isa<PHINode>(I) 

      for(Loop::block_iterator BI = L.block_begin(); BI != L.block_end(); ++BI) {
        llvm::BasicBlock *BB = *BI;
        for(auto &I : *BB) {
          bool isLoopInvariant = true;
          for(auto &op : I.operands()) {
            // Le istruzioni come branch e return non sono loop invariant per definizione
            // dato che alterando il control flow spostandole farebbero perdere di senso il programma
            // e dobbiamo controllare che non vengano inserite nel nostro vettore di istruzioni loop invariant
            if (I.isTerminator() || I.mayHaveSideEffects()) {
              continue;
            }
            // Check if the operator is a constant
            if(Instruction* inst = dyn_cast<Instruction>(op)) {
              if(L.contains(inst->getParent()) && !contains(inst, LoopInvInst) && isAnalized(inst, LoopInvInst)) {
                // TODO qui la funzione contains non è sufficiente, potrebbe non essere nella nostra lista perchè non è stata ancora anlizzata
                // Se l'istruzione non è ancora stata analizzata, andremo nell'else if.
                isLoopInvariant = false;
                break;
              }else if(!isAnalized(inst, LoopInvInst)) {
                
                }
              } else {
                // Se l'istruzione è loop invariant, la aggiungiamo alla lista
                if(!contains(inst, LoopInvInst)) {
                  LoopInvInst.push_back(inst);

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
        // TODO spostare le itruzioni :: ci pensiamo noi
      }
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
