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

using namespace llvm;

//-----------------------------------------------------------------------------
namespace {


// New PM implementation
struct AlgebraicIdentity: PassInfoMixin<AlgebraicIdentity> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {

    Value *isAdd(Instruction &Inst) {
        if (Inst.getOpcodeName() == std::string("add")) {
            outs() << "Istruzione add detectata! \n";
            Value *Operand1 = Inst.getOperand(0);
            Value *Operand2 = Inst.getOperand(1);
    
            // Controlla se uno degli operandi è 0
            if (ConstantInt *C = dyn_cast<ConstantInt>(Operand1)) {
                if (C->getSExtValue() == 0) {
                    outs() << "Trovato! L'istruzione ha un operando che è una costante intera uguale a 0.\n";
                    return Operand2;
                }
            }
            if (ConstantInt *C = dyn_cast<ConstantInt>(Operand2)) {
                if (C->getSExtValue() == 0) {
                    outs() << "Trovato! L'istruzione ha un operando che è una costante intera uguale a 0.\n";
                    return Operand1; 
                }
            }
        }
        return nullptr;
    }
    
    Value *isMul(Instruction &Inst) {
        if (Inst.getOpcodeName() == std::string("mul")) {
            outs() << "Istruzione mul detectata! \n";
            Value *Operand1 = Inst.getOperand(0);
            Value *Operand2 = Inst.getOperand(1);
    
            // Controlla se uno degli operandi è 1
            if (ConstantInt *C = dyn_cast<ConstantInt>(Operand1)) {
                if (C->getSExtValue() == 1) {
                    outs() << "Trovato! L'istruzione ha un operando che è una costante intera uguale ad 1.\n";
                    return Operand2; // Ritorna l'altro operando
                }
            }
            if (ConstantInt *C = dyn_cast<ConstantInt>(Operand2)) {
                if (C->getSExtValue() == 1) {
                    outs() << "Trovato! L'istruzione ha un operando che è una costante intera uguale ad 1.\n";
                    return Operand1; // Ritorna l'altro operando
                }
            }
        }
        return nullptr; // Nessun operando soddisfa la condizione
    }

    bool runOnBasicBlock(BasicBlock &B) {
        for (auto Iter = B.begin(); Iter != B.end();) {
            Instruction &Inst = *Iter;
            outs() << "Analizzando istruzione: " << Inst << "\n";
    
            Value* Replacement = nullptr;
    
            if ((Replacement = isMul(Inst)) || (Replacement = isAdd(Inst))) {
    
                outs() << "Sostituendo tutti gli usi dell'istruzione con: " << *Replacement << "\n";
                Inst.replaceAllUsesWith(Replacement);
                outs() << "Rimuovendo istruzione: " << Inst << "\n";
                Iter = Inst.eraseFromParent();
            } else {
                ++Iter;
            }
        }
        return false;
    }

    bool runOnFunction(Function &F) {
        bool Transformed = false;
      
        for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
          if (runOnBasicBlock(*Iter)) {
            Transformed = true;
          }
        }
      
        return Transformed;
      }

    if (runOnFunction(F)) {
        return PreservedAnalyses::none(); // Indica che il pass ha modificato il codice
    }
    return PreservedAnalyses::all(); // Indica che il pass non ha modificato nulla
  }
  // Without isRequired returning true, this pass will be skipped for functions
  // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
  // all functions with optnone.
  static bool isRequired() { return true; }
};

// New PM implementation
struct StrengthReduction: PassInfoMixin<StrengthReduction> {
    // Main entry point, takes IR unit to run the pass on (&F) and the
    // corresponding pass manager (to be queried if need be)
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        int isUpperPowerOfTwo(int64_t Val) {
            if (Val > 0 && ((Val + 1) & Val) == 0) {
                outs() << "Il numero è una potenza di 2+1.\n";
                log = Log2_64(Val);
                return log;
            }
            return 0;
        }
        
        int isLowerPowerOfTwo(int64_t Val) {
            if (Val > 0 && ((Val - 1) & Val) == 0) {
                outs() << "Il numero è una potenza di 2-1.\n";
                log = Log2_64(Val);
                return log+1;
            }
            return 0;
        }
        
        Value *isDiv(Instruction &Inst) {
            if (Inst.getOpcodeName() == std::string("sdiv")) {
                outs() << "Istruzione add detectata! \n";
                Value *Operand1 = Inst.getOperand(0);
                Value *Operand2 = Inst.getOperand(1);
        
                for (unsigned i = 0; i < Inst.getNumOperands(); ++i) {
                    Value *Operand = Inst.getOperand(i);
        
                    // Verifica se l'operando è una costante intera
                    if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) {
                        int64_t Val = C->getSExtValue(); // Ottieni il valore della costante
        
                        if(log = isUpperPowerOfTwo(Val) != 0) {
                            Value *Shifted = BinaryOperator::CreateShl(Inst.getOperand(0), 
                            ConstantInt::get(C->getType(), log), "", &Inst);
                            Value *Optimized = BinaryOperator::CreateSub(Shifted, Inst.getOperand(0), "", &Inst);
                            return Optimized;
                        }else if (log = isLowerPowerOfTwo(Val) != 0) {
                            Value *Shifted = BinaryOperator::CreateShl(Inst.getOperand(0), 
                                ConstantInt::get(C->getType(), log), "", &Inst);
                            Value *Optimized = BinaryOperator::CreateSub(Shifted, Inst.getOperand(0), "", &Inst);
                            return Optimized;
                        }
                    }
                }
            }
            return nullptr; // Nessun operando soddisfa la condizione
        }
        
        Value *isMul(Instruction &Inst) {
            if (Inst.getOpcodeName() == std::string("mul")) {
                outs() << "Istruzione add detectata! \n";
                Value *Operand1 = Inst.getOperand(0);
                Value *Operand2 = Inst.getOperand(1);
        
                for (unsigned i = 0; i < Inst.getNumOperands(); ++i) {
                    Value *Operand = Inst.getOperand(i);
        
                    // Verifica se l'operando è una costante intera
                    if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) {
                        int64_t Val = C->getSExtValue(); // Ottieni il valore della costante
        
                        if(log = isUpperPowerOfTwo(Val) != 0) {
                            Value *Shifted = BinaryOperator::CreateShl(Inst.getOperand(0), 
                            ConstantInt::get(C->getType(), log), "", &Inst);
                            Value *Optimized = BinaryOperator::CreateSub(Shifted, Inst.getOperand(0), "", &Inst);
                            return Optimized;
                        }else if (log = isLowerPowerOfTwo(Val) != 0) {
                            Value *Shifted = BinaryOperator::CreateShl(Inst.getOperand(0), 
                                ConstantInt::get(C->getType(), log), "", &Inst);
                            Value *Optimized = BinaryOperator::CreateSub(Shifted, Inst.getOperand(0), "", &Inst);
                            return Optimized;
                            
                        }
                        
                    }
                }
            }
            return nullptr; // Nessun operando soddisfa la condizione
        }
        
        bool runOnBasicBlock(BasicBlock &B) {
            for (auto Iter = B.begin(); Iter != B.end();) {
                Instruction &Inst = *Iter;
        
                // Debug: stampa l'istruzione corrente
                outs() << "Analizzando istruzione: " << Inst << "\n";
        
                Value* Replacement = nullptr;
        
                // Controlla se l'istruzione è un'addizione o una moltiplicazione
                if ((Replacement = isMul(Inst)) || (Replacement = isDiv(Inst))) {
                    // Debug: stampa il valore di sostituzione
                    outs() << "Sostituendo tutti gli usi dell'istruzione con: " << *Replacement << "\n";
        
                    // Sostituisci tutti gli usi dell'istruzione con il valore ritornato
                    Inst.replaceAllUsesWith(Replacement);
        
                    // Debug: verifica che l'istruzione non abbia più usi
                    if (!Inst.use_empty()) {
                        outs() << "Errore: l'istruzione ha ancora usi dopo replaceAllUsesWith!\n";
                    }
        
                    // Rimuovi l'istruzione e aggiorna l'iteratore
                    outs() << "Rimuovendo istruzione: " << Inst << "\n";
                    Iter = Inst.eraseFromParent();
                } else {
                    ++Iter; // Passa all'istruzione successiva
                }
            }
            return false; // Ritorna false se non ci sono trasformazioni significative
        }
        
        bool runOnFunction(Function &F) {
          bool Transformed = false;
        
          for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
            if (runOnBasicBlock(*Iter)) {
              Transformed = true;
            }
          }
        
          return Transformed;
        }

        if (runOnFunction(F)) {
            return PreservedAnalyses::none(); // Indica che il pass ha modificato il codice
        }
        return PreservedAnalyses::all(); // Indica che il pass non ha modificato nulla
    }
    // Without isRequired returning true, this pass will be skipped for functions
    // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
    // all functions with optnone.
    static bool isRequired() { return true; }
  };
   
} // namespace
  



//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getTestPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "Algebraic Identity", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "algebraic-identity") {
                    FPM.addPass(AlgebraicIdentity());
                    return true;
                  }else if(Name == "strength-reduction") {
                    FPM.addPass(StrengthReduction());
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
llvmGetPassPluginInfo() {
  return getTestPassPluginInfo();
}
