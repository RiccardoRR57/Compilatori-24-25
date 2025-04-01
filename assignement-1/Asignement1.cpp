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
        // Mi trovo in una operazione
      }
      else if (BinaryOperator *ope = dyn_cast<BinaryOperator>(&I))
      {
        // Controllo se questa è una divisione/unsigned divisione
        if (ope->getOpcode() == Instruction::SDiv || ope->getOpcode() == Instruction::UDiv)
        {
          // Controllo se l'operando 1 è una costante
          if (ConstantInt *c = dyn_cast<ConstantInt>(ope->getOperand(1)))
          {
            if (c->getValue() == 1)
            {
              // Rimpiazzo gli usi se questa costante è uguale a 1
              ope->replaceAllUsesWith(ope->getOperand(0));
              return true;
            }
          }
          // Se non è una divisione controllo se è una sottrazione
        }
        else if (ope->getOpcode() == Instruction::Sub)
        {
          // Controllo se l'operando 1 è una costante
          if (ConstantInt *c = dyn_cast<ConstantInt>(ope->getOperand(1)))
          {
            // Controllo se la costante è uguale a 0
            if (c->getValue() == 0)
            {
              // Rimpiazzo tutti gli usi
              ope->replaceAllUsesWith(ope->getOperand(0));
              return true;
            }
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
      // Mi trovo in una moltiplicazione
      if (MulOperator *mul = dyn_cast<MulOperator>(&I))
      {
        // Controllo il valore del primo operando
        if (ConstantInt *c = dyn_cast<ConstantInt>(mul->getOperand(0)))
        {
          // Controllo se è una potenza di 2
          if (c->getValue().isPowerOf2())
          {
            // Calcoliamo il log
            Value *shift_val = ConstantInt::get(c->getType(), c->getValue().logBase2());
            // Creiamo l'istruzione
            Instruction *shift = BinaryOperator::Create(Instruction::Shl, mul->getOperand(1), shift_val);
            // Inseriamo e rimuoviamo l'istruzione NON ottimizzata
            shift->insertBefore(&I);
            I.replaceAllUsesWith(shift);
            return true;
          }
          // Controllo se siamo a disstanza -1 da potenza di 2 più vicina
          else if ((c->getValue()+1).isPowerOf2())
          {
            // Calcoliamo il log addizionato di 1
            Value *shift_val = ConstantInt::get(c->getType(), c->getValue().logBase2() + 1);
            // Creiamo l'istruzione
            Instruction *shift = BinaryOperator::Create(Instruction::Shl, mul->getOperand(1), shift_val);
            // Inseriamo l'istruzione
            shift->insertBefore(&I);
            // Creiamo l'istruzione finale
            Instruction *sub = BinaryOperator::Create(Instruction::Sub, shift, mul->getOperand(1));
            // Inseriamo l'istruzione di add
            sub->insertBefore(&I);
            // Rimuoviamo l'istruzione NON ottimizzata
            I.replaceAllUsesWith(sub);
            
            return true;
          }
          // Controllo se siamo a distanza +1 da potenza di 2 più vicina
          else if ((c->getValue() - 1).isPowerOf2())
          {
            // Calcoliamo il log
            Value *shift_val = ConstantInt::get(c->getType(), c->getValue().logBase2());
            // Creiamo l'istruzione
            Instruction *shift = BinaryOperator::Create(Instruction::Shl, mul->getOperand(1), shift_val);
            // Inseriamo l'istruzione
            shift->insertBefore(&I);
            // Creiamo l'istruzione finale
            Instruction *add = BinaryOperator::Create(Instruction::Add, shift, mul->getOperand(1));
            // Inseriamo l'istruzione di sub
            add->insertBefore(&I);
            // Rimuoviamo l'istruzione NON ottimizzata
            I.replaceAllUsesWith(add);
            return true;
          }
        }
        // Controllo il valore del secondo operando
        else if (ConstantInt *c = dyn_cast<ConstantInt>(mul->getOperand(1)))
        {
          // Controllo se è una potenza di 2
          if (c->getValue().isPowerOf2())
          {
            // Calcoliamo il log
            Value *shift_val = ConstantInt::get(c->getType(), c->getValue().logBase2());
            // Creiamo l'istruzione
            Instruction *shift = BinaryOperator::Create(Instruction::Shl, mul->getOperand(0), shift_val);
            // Inseriamo l'istruzione e rimuoviamo quella NON ottimizzata
            shift->insertBefore(&I);
            I.replaceAllUsesWith(shift);
            return true;
          }
          else if ((c->getValue()+1).isPowerOf2())
          {
            // Calcoliamo il log addizionato di 1
            Value *shift_val = ConstantInt::get(c->getType(), c->getValue().logBase2() + 1);
            // Creiamo l'istruzione
            Instruction *shift = BinaryOperator::Create(Instruction::Shl, mul->getOperand(0), shift_val);
            // Inseriamo e rimuoviamo l'istruzione NON ottimizzata
            shift->insertBefore(&I);
            // Creiamo l'istruzione finale
            Instruction *sub = BinaryOperator::Create(Instruction::Sub, shift, mul->getOperand(0));
            // Inseriamo l'istruzione di add
            sub->insertBefore(&I);
            I.replaceAllUsesWith(sub);
            return true;
          }
          else if ((c->getValue() - 1).isPowerOf2())
          {
            // Calcoliamo il log
            Value *shift_val = ConstantInt::get(c->getType(), c->getValue().logBase2());
            // Creiamo l'istruzione
            Instruction *shift = BinaryOperator::Create(Instruction::Shl, mul->getOperand(0), shift_val);
            // Inseriamo l'istruzione
            shift->insertBefore(&I);
            // Creiamo l'istruzione finale
            Instruction *add = BinaryOperator::Create(Instruction::Add, shift, mul->getOperand(0));
            // Inseriamo l'istruzione di add
            add->insertBefore(&I);
            // Rimuoviamo l'istruzione NON ottimizzata
            I.replaceAllUsesWith(add);
            return true;
          }
        }
        // Mi trovo in una operazione
        else if (BinaryOperator *div = dyn_cast<BinaryOperator>(&I))
        {
          // Controllo se è una divisione
          if (div->getOpcode() != Instruction::SDiv && div->getOpcode() != Instruction::UDiv)
            return false;
          // Controllo il primo operando
          else if (ConstantInt *c = dyn_cast<ConstantInt>(div->getOperand(1)))
          {
            // Controllo se l'operando è una potenza di 2
            if (c->getValue().isPowerOf2())
            {
              Value *shift_val = ConstantInt::get(c->getType(), c->getValue().logBase2());
              Instruction *shift = BinaryOperator::Create(Instruction::AShr, div->getOperand(0), shift_val);
              shift->insertBefore(&I);
              I.replaceAllUsesWith(shift);
              return true;
            }
          }
        }
        return false;
      }
      return false;
    }
    static bool isRequired() { return true; }
  };

  // Multi Instruction optimization pass
  struct MultiInstr : PassInfoMixin<MultiInstr>
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

    bool runOnInstruction(Instruction &I) {
      // Mi trovo in una add
      if(AddOperator *add = dyn_cast<AddOperator>(&I)) 
      {
        // Controllo se l'operando 0 è una costante
        if(ConstantInt *c = dyn_cast<ConstantInt>(add->getOperand(0)))
        {
          // Reperisco istruzione successiva
          Instruction *next = I.getNextNode();
          while(next) {
            errs() << "Analizzo operazione " << *next << '\n';
            // Controllo se è sottrazione
            if(SubOperator *sub = dyn_cast<SubOperator>(next)) {
              // Controllo se si verificano le condizioni per ottimizzazione
              if(add == sub->getOperand(0) && (add->getOperand(0) == sub->getOperand(1)))
              {
                errs() << "Rimuovo operazione " << *next << '\n';
                // Rimpiazziamo tutti gli usi della sottrazione
                sub->replaceAllUsesWith(add->getOperand(1));
                
                // Cancelliamo la sottrazione
                Instruction *temp = next->getPrevNode();
                next->eraseFromParent();
                next = temp;
              }
            }
            next = next->getNextNode();
          }
          errs() << "finito ciclo\n";
          return false;
        }
        // Controllo se l'operando 1 è una costante
        if(ConstantInt *c = dyn_cast<ConstantInt>(add->getOperand(1)))
        {
          // Reperisco istruzione successiva
          Instruction *next = I.getNextNode();
          // Controllo se è sottrazione
          if(SubOperator *sub = dyn_cast<SubOperator>(next)) {
            // Controllo se si verificano le condizioni per ottimizzazione
            if(add == sub->getOperand(0) && (add->getOperand(1) == sub->getOperand(1)))
            {
              // Rimpiazziamo tutti gli usi della sottrazione
              sub->replaceAllUsesWith(add->getOperand(0));
              // Cancelliamo la sottrazione
              next->eraseFromParent();
              return false;
            }
          }
        }
      }
      if(SubOperator *sub = dyn_cast<SubOperator>(&I)) 
      {
        // Controllo se l'operando 0 è una costante
        // Effettuiamo controllo solo sul secondo operando dato che se la costante fosse 
        // al primo non si potrebbe effettuare l'ottimizzazione
        if(ConstantInt *c = dyn_cast<ConstantInt>(sub->getOperand(1)))
        {
          // Reperisco istruzione successiva
          Instruction *next = I.getNextNode();
          // Controllo se è sottrazione
          if(AddOperator *add = dyn_cast<AddOperator>(next)) {
            // Controllo se si verificano le condizioni per ottimizzazione
            if(sub == add->getOperand(0) && (sub->getOperand(1) == add->getOperand(1)))
            {
              // Rimpiazziamo tutti gli usi dell'addizione
              add->replaceAllUsesWith(sub->getOperand(0));
              // Cancelliamo l'addizione
              next->eraseFromParent();
              return false;
            }
            if(sub == add->getOperand(1) && (sub->getOperand(1) == add->getOperand(0)))
            {
              // Rimpiazziamo tutti gli usi dell'addizione
              add->replaceAllUsesWith(sub->getOperand(0));
              // Cancelliamo l'addizione
              next->eraseFromParent();
              return false;
            }
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
llvm::PassPluginLibraryInfo
getOpts1()
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
                  else if (Name == "multi-instruction") 
                  {
                    FPM.addPass(MultiInstr());
                    return true;
                  }
                  else if (Name == "all")
                  {
                    FPM.addPass(AlgIde());
                    FPM.addPass(StrRed());
                    FPM.addPass(MultiInstr());
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
