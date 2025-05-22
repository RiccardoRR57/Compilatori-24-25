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
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/DependenceAnalysis.h"

using namespace llvm;

//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.
namespace
{
  // New PM implementation
  struct LoopFusion1 : PassInfoMixin<LoopFusion1>
  {
    void runOnLoop(Loop &L, DominatorTree &DT) {

    }

    /**
     * Ritorna true se i due loop sono adiacenti
     * false se non sono adiacenti oppure il primo ha più uscite
     * 
     */
    bool areAdjacent(Loop* L1, Loop* L2) {
      outs() << "Controllo se i loop sono adiacenti\n";
      //La funzione getExitBlock ritorna non null solamente se vi è una sola uscita, in caso contario ritorna null.
      BasicBlock* ExitL1 = L1->getExitBlock();
      BasicBlock* EntryL2;

      if(L2->isGuarded()){ 
        // Prendiamo il branch del guard del Loop in caso sia guarded e poi prendiamo il BB tramite getParent().
        EntryL2 = L2->getLoopGuardBranch()->getParent()->getPrevNode();
      } else {
        //Altrimenti prendiamo il preheader del loop come entry block.
        EntryL2 = L2->getLoopPreheader();  
      }
      return EntryL2 == ExitL1;
    }
    /**
     * Ritrona true se i due loop sono CFG equivalenti
     * false se non sono CFG equivalenti
     *  
     */
    bool areCFGEquivalent(Loop* L1, Loop* L2, DominatorTree &DT, PostDominatorTree &PDT) {

      // Istanziamo due BasickBlock che nel caso in cui non siano guarded diventeranno
      // l'header del loop mentre nel caso opposto saranno i BB contenenti il branch del guard.
      BasicBlock* BlockL1;
      BasicBlock* BlockL2;

      // Controlliamo se entrambi i loop sono guarder, evitiamo il controllo su guardia 
      // singola in quanto questo caso preclude l'ipotesi di CFG equivalenza
      if(L1->isGuarded() && L2->isGuarded()) {
        outs() << "Entrambi i loop sono guarded\n";
        // Controlliamo se entrambe le condizioni delle guardie sono uguali
        // In caso contrario non verrà rispettata la proprietà di CFG equivalenza
        if(L1->getLoopGuardBranch()->isSameOperationAs(L2->getLoopGuardBranch())) {
          outs() << "La condizione è uguale\n";
          BlockL1 = L1->getLoopGuardBranch()->getParent();
          BlockL2 = L2->getLoopGuardBranch()->getParent();
        }
        else {
          outs() << "La condizione è diversa\n";
          return false;
        }
      }
      // Se entrambi non sono guarded allora possiamo assegnare a BlockL1 e BlockL2
      // l'header del loop rispettivo e procedere al controllo della dominanza.
      else if(!L1->isGuarded() && !L2->isGuarded()) {
        outs() << "Entrambi i loop non sono guarded\n";
        BlockL1 = L1->getHeader();
        BlockL2 = L2->getHeader();
      }
      if(DT.dominates(BlockL1, BlockL2)) {
        outs() << "Il primo loop domina il secondo\n";
      }
      if(PDT.dominates(BlockL2, BlockL1)) {
        outs() << "Il secondo loop domina il primo\n";
      }
      return DT.dominates(BlockL1, BlockL2) && PDT.dominates(BlockL2, BlockL1);
    }

    bool sameIterationNumber(Loop* L1, Loop* L2, ScalarEvolution* SE) {

      // Numero di iterazioni del loop1 (0 se indefinito)
      unsigned int L1TripCount = SE->getSmallConstantTripCount(L1);
      // Numero di iterazioni del loop2 (0 se indefinito)
      unsigned int L2TripCount = SE->getSmallConstantTripCount(L2);
      
      // Se almeno uno dei due trip count non è calcolabile ritorniamo false
      if(L1TripCount == 0 || L2TripCount == 0) {
        outs() << "Almeno un trip count non è calcolabile\n";
        return false;
      }
      
      if(L1TripCount == L2TripCount) {
        return true;
      }

      return false;
      
    }

    bool hasDependence(Loop* L1, Loop* L2, DependenceInfo* DI, ScalarEvolution* SE) {
      
      // Scorro i basic block del loop1
      for(auto* BB : L1->blocks()) {
        // Scorro le istruzioni del loop1
        for(auto I = BB->begin(); I != BB->end(); ++I) {
          // Controllo solamnete le istruzioni interessate
          if(StoreInst *store = dyn_cast<StoreInst>(I)) {

            outs() << "trovata una store: " << *store << '\n';
            // Ricavo L'istruzione usata come operando dalla store
            Instruction* getElemPtrS = dyn_cast<Instruction>(store->getOperand(1));
            // Ricavo base address e offSerT.
            Value* baseS = getElemPtrS->getOperand(0);
            Value* offsetS = getElemPtrS->getOperand(2);
            outs() << "base: " << *baseS << "\n";
            outs() << "offset" << *offsetS << "\n";
            outs() << "-----------------------------------------" << "\n";

            // Scorro i basic block del loop2
            for(auto* BB : L2->blocks()) {
              // Scorro le istruzioni del loop2
              for(auto I = BB->begin(); I != BB->end(); ++I) {
                // Controllo solamnete le istruzioni interessate
                if(LoadInst *load = dyn_cast<LoadInst>(I)) {
                  outs() << "trovata una load: " << *load << '\n';
                  // Ricavo L'istruzione usata come operando dalla load
                  Instruction* getElemPtrL = dyn_cast<Instruction>(load->getOperand(0));
                  Value* baseL = getElemPtrL->getOperand(0);
                  Value* offsetL = getElemPtrL->getOperand(2);
                  outs() << "base: " << *baseL << "\n";
                  outs() << "offset" << *offsetL << "\n";

                  // Controllo se hanno la stessa base
                  if(baseS == baseL) {
                    outs() << "La store e la load si riferiscono allo stesso array\n";

                    Dependence *dep = DI->depends(store, load, true).release();
    
                    if(dep) {
                      outs()<< dep->getDistance(0)<< "\n";

                      return true;
                    }else {
                      return false;
                    }



/*
                    // Calcolo la scalar evolution di load store e la differenza tra queste
                    const SCEV* storeSCEV = SE->getSCEV(offsetS);
                    outs()<< *storeSCEV << "calcolo offset store \n";
                    const SCEV* loadSCEV = SE->getSCEV(offsetL);
                    outs() << *loadSCEV << "calcolo offset load \n";
                    const SCEV* diff = SE->getMinusSCEV(storeSCEV, loadSCEV);
                    outs() << *diff << "calcolo diff \n";
                    
                    SE->

                    // Calcolo la distanza di dipendenza
                    outs() << "differenza: " << SE->getUnsignedRange(diff) << '\n';
                    if(SE->getUnsignedRangeMin(diff).isNegative())
                    
                    {
                      outs() << "Dipendenze a distanza sconosciuta o negativa\n";
                      return true;
                    }
*/



                  }
                }
              }
            }
          }
        }
      }
    
      return false;
    }

    // Main entry point, takes IR unit to run the pass on (&F) and the
    // corresponding pass manager (to be queried if need be)
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM)
    {
      LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
      DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
      PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
      ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
      DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

      //Sapendo che i lo
      // op sono solo due, prendo i primi due
      std::vector<Loop*> Loops = LI.getTopLevelLoops();
      if(Loops.size() < 2) {
        outs() << "Non ci sono abbastanza loop\n";
        return PreservedAnalyses::all();
      }
      Loop* L1 = Loops[1];
      Loop* L2 = Loops[0];
      
      outs() << "INIZIO CONTROLLO DI ADIACENZA" << "\n";
      outs() << "-----------------------------------------" << "\n";

      if(areAdjacent(L1, L2)) {
        outs() << "I loop sono adiacenti\n \n";
      } else {
        outs() << "I loop non sono adiacenti\n \n";
      }

      outs() << "INIZIO CONTROLLO DI CFG EQUIVALENZA" << "\n";
      outs() << "-----------------------------------------" << "\n";
      
      if(areCFGEquivalent(L1,L2, DT, PDT)) {
        outs() << "I loop sono CFG equivalenti\n\n";
      } else {
        outs() << "I loop non sono CFG equivalenti\n\n";
      } 

      outs() << "INIZIO CONTROLLO SUL NUMERO DI ITERAZIONI" << "\n";
      outs() << "-----------------------------------------" << "\n";

      if(sameIterationNumber(L1,L2,&SE)){
        outs() << "I loop hanno lo stesso numero di iterazioni\n\n";
      } else {
        outs() << "I loop non hanno lo stesso numero di iterazioni\n\n";
      }

      outs() << "INIZIO CONTROLLO SULLE DIPENDENZE A DISTANZA NEGATIVA" << "\n";
      outs() << "-----------------------------------------" << "\n";

      if(hasDependence(L1, L2, &DI, &SE)) {
        outs() << "I loop hanno dipendenze\n\n";
      } else {
        outs() << "I loop non hanno dipendenze\n\n";
      }

      outs() << "-----------------------------------------" << "\n";

      


      
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
  return {LLVM_PLUGIN_API_VERSION, "LoopFusion1", LLVM_VERSION_STRING,
          [](PassBuilder &PB)
          {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                  if (Name == "loop-fusion1") 
                  {
                    FPM.addPass(LoopFusion1());
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
