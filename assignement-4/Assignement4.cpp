//=============================================================================
// FILE:
//    TestPass.cpp
//
// DESCRIPTION:
//    Pass LLVM per la fusione di loop adiacenti con analisi delle dipendenze
//    Implementa controlli di adiacenza, CFG equivalenza e dipendenze negative
//
// USAGE:
//    New PM
//      opt -load-pass-plugin=<path-to>libTestPass.so -passes="loop-fusion1" `\`
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
#include "llvm/Analysis/ScalarEvolution.h" // Per analisi matematica delle espressioni
#include "llvm/Analysis/DependenceAnalysis.h" // Per rilevare dipendenze tra accessi memoria

using namespace llvm;

//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
namespace
{
  // Struttura principale del pass per la fusione di loop
  // Eredita da PassInfoMixin per integrarsi nel nuovo Pass Manager di LLVM
  struct LoopFusion1 : PassInfoMixin<LoopFusion1>
  {
    /**
     * CONDIZIONE 1: ADIACENZA FISICA NEL CFG
     * Verifica se due loop sono fisicamente adiacenti nel CFG
     * Un loop è adiacente se l'uscita del primo coincide con l'ingresso del secondo
     * Questa è una condizione necessaria per la fusione sicura dei loop
     */
    bool areAdjacent(Loop* L1, Loop* L2) {
      outs() << "Controllo se i loop sono adiacenti\n";
      
      // getExitBlock() ritorna null se ci sono multiple uscite
      // Per la fusione, è necessario che ogni loop abbia una sola uscita
      BasicBlock* ExitL1 = L1->getExitBlock();
      BasicBlock* EntryL2;

      // Gestione dei loop con guard: alcuni loop hanno una condizione di guardia
      // che precede il loop vero e proprio per ottimizzazioni del compilatore
      if(L2->isGuarded()){ 
        // Se il loop ha una guardia, consideriamo il blocco precedente al guard
        // come punto di ingresso effettivo
        EntryL2 = L2->getLoopGuardBranch()->getParent()->getPrevNode();
      } else {
        // Altrimenti usiamo il preheader standard (blocco che precede l'header del loop)
        EntryL2 = L2->getLoopPreheader();  
      }
      
      // Due loop sono adiacenti se l'uscita del primo è l'ingresso del secondo
      return EntryL2 == ExitL1;
    }

    /**
     * CONDIZIONE 2: CFG EQUIVALENZA
     * Verifica CFG equivalenza: stesso pattern di controllo di flusso
     * Usa dominanza diretta e post-dominanza per garantire equivalenza
     * Due loop sono CFG equivalenti se hanno la stessa struttura di controllo
     */
    bool areCFGEquivalent(Loop* L1, Loop* L2, DominatorTree &DT, PostDominatorTree &PDT) {

      // Blocchi di riferimento per il controllo di dominanza
      // Questi saranno i blocchi su cui verificare le relazioni di dominanza
      BasicBlock* BlockL1;
      BasicBlock* BlockL2;

      // Caso 1: entrambi hanno guardie - devono essere identiche
      // I loop con guardie hanno condizioni di ingresso che devono essere uguali
      if(L1->isGuarded() && L2->isGuarded()) {
        outs() << "Entrambi i loop sono guarded\n";
        
        // Le condizioni delle guardie devono essere sintatticamente uguali
        // isSameOperationAs verifica che le istruzioni siano identiche
        if(L1->getLoopGuardBranch()->isSameOperationAs(L2->getLoopGuardBranch())) {
          outs() << "La condizione è uguale\n";
          // Usiamo i blocchi che contengono le guardie per l'analisi di dominanza
          BlockL1 = L1->getLoopGuardBranch()->getParent();
          BlockL2 = L2->getLoopGuardBranch()->getParent();
        }
        else {
          outs() << "La condizione è diversa\n";
          return false; // Condizioni diverse = non CFG equivalenti
        }
      }
      // Caso 2: nessuno ha guardie - usiamo gli header
      // Gli header sono i blocchi di ingresso principale dei loop
      else if(!L1->isGuarded() && !L2->isGuarded()) {
        outs() << "Entrambi i loop non sono guarded\n";
        BlockL1 = L1->getHeader();
        BlockL2 = L2->getHeader();
      }
      
      // ANALISI DI DOMINANZA:
      // Un blocco A domina B se ogni cammino dall'entrata a B passa per A
      // Un blocco A post-domina B se ogni cammino da B all'uscita passa per A
      
      // Verifica dominanza: L1 deve dominare L2 (L1 viene prima nel flusso)
      if(DT.dominates(BlockL1, BlockL2)) {
        outs() << "Il primo loop domina il secondo\n";
      }
      // Verifica post-dominanza: L2 deve post-dominare L1 (L1 deve portare a L2)
      if(PDT.dominates(BlockL2, BlockL1)) {
        outs() << "Il secondo loop domina il primo\n";
      }
      
      // CFG equivalenza richiede entrambe le condizioni
      return DT.dominates(BlockL1, BlockL2) && PDT.dominates(BlockL2, BlockL1);
    }

    /**
     * CONDIZIONE 3: STESSO NUMERO DI ITERAZIONI
     * Verifica che i loop abbiano lo stesso numero di iterazioni
     * Necessario per garantire che la fusione mantenga la semantica originale
     */
    bool sameIterationNumber(Loop* L1, Loop* L2, ScalarEvolution* SE) {

      // Trip count: numero costante di iterazioni calcolato staticamente
      // ScalarEvolution può determinare il numero di iterazioni per loop semplici
      // Ritorna 0 se non riesce a calcolare un valore costante
      unsigned int L1TripCount = SE->getSmallConstantTripCount(L1);
      unsigned int L2TripCount = SE->getSmallConstantTripCount(L2);
      
      outs() << "Trip count L1: " << L1TripCount << "\n";
      outs() << "Trip count L2: " << L2TripCount << "\n";

      // Se non riusciamo a calcolare almeno uno, fusione non sicura
      // Senza sapere il numero di iterazioni, non possiamo garantire correttezza
      if(L1TripCount == 0 || L2TripCount == 0) {
        outs() << "Almeno un trip count non è calcolabile\n";
        return false;
      }
      
      // I trip count devono essere identici per una fusione sicura
      if(L1TripCount == L2TripCount) {
        return true;
      }

      return false;
    }

    /**
     * CONDIZIONE 4: ANALISI DIPENDENZE NEGATIVE
     * Analizza dipendenze negative tra accessi memoria dei due loop
     * Una dipendenza negativa si verifica quando L2 legge da una locazione
     * che L1 scriverà in una iterazione futura, violando l'ordine di esecuzione
     */
    bool hasDependence(Loop* L1, Loop* L2, DependenceInfo* DI, ScalarEvolution* SE) {
      
      // STRATEGIA: cercare pattern Write-After-Read (WAR) tra i loop
      // Scorriamo tutte le scritture in L1 e tutte le letture in L2
      
      // Scorro tutti i basic block del primo loop
      for(auto* BB : L1->blocks()) {
        // Analizzo ogni istruzione cercando store (scritture in memoria)
        for(auto I = BB->begin(); I != BB->end(); ++I) {
          if(StoreInst *store = dyn_cast<StoreInst>(I)) {

            outs() << "trovata una store: " << *store << '\n';
            
            // ANALISI DELL'ACCESSO MEMORIA DELLA STORE:
            // Estraggo l'istruzione GEP (GetElementPtr) per l'indirizzamento
            // GEP calcola l'indirizzo di un elemento in una struttura dati
            Instruction* getElemPtrS = dyn_cast<Instruction>(store->getOperand(1));
            
            // Base dell'array (primo operando) e offset per l'accesso (terzo operando)
            // Operando 0: base pointer, Operando 2: indice dell'array
            Value* baseS = getElemPtrS->getOperand(0);
            Value* offsetS = getElemPtrS->getOperand(2);
            outs() << "base: " << *baseS << "\n";
            outs() << "offset" << *offsetS << "\n";
            outs() << "-----------------------------------------" << "\n";

            // Scorro il secondo loop cercando load (letture da memoria)
            for(auto* BB : L2->blocks()) {
              for(auto I = BB->begin(); I != BB->end(); ++I) {
                if(LoadInst *load = dyn_cast<LoadInst>(I)) {
                  outs() << "trovata una load: " << *load << '\n';
                  
                  // ANALISI DELL'ACCESSO MEMORIA DELLA LOAD:
                  // Estraggo base e offset della load con la stessa logica
                  Instruction* getElemPtrL = dyn_cast<Instruction>(load->getOperand(0));
                  Value* baseL = getElemPtrL->getOperand(0);
                  Value* offsetL = getElemPtrL->getOperand(2);
                  outs() << "base: " << *baseL << "\n";
                  outs() << "offset" << *offsetL << "\n";

                  // CONTROLLO DIPENDENZA: stessi array = possibile conflitto
                  if(baseS == baseL) {
                    outs() << "La store e la load si riferiscono allo stesso array\n";

                    // ANALISI SCALAR EVOLUTION:
                    // SCEV (Scalar Evolution) analizza come cambiano i valori nelle iterazioni
                    // Permette di capire pattern matematici negli indici degli array
                    const SCEV* storeSCEV = SE->getSCEV(offsetS);
                    outs()<< *storeSCEV << "calcolo offset store \n";
                    const SCEV* loadSCEV = SE->getSCEV(offsetL);
                    outs() << *loadSCEV << "calcolo offset load \n";

                    // CAST A AddRecExpr: pattern ricorsivi del tipo {start, +, step}
                    // Rappresentano sequenze come start, start+step, start+2*step, ...
                    const SCEVAddRecExpr* storeARE = dyn_cast<SCEVAddRecExpr>(storeSCEV);
                    const SCEVAddRecExpr* loadARE = dyn_cast<SCEVAddRecExpr>(loadSCEV);

                    // Se entrambi sono pattern ricorsivi, analizzo la dipendenza matematicamente
                    if(storeARE && loadARE) {
                      // ESTRAZIONE PARAMETRI DEL PATTERN:
                      // Start: valore iniziale della sequenza
                      const SCEV* storeStart = storeARE->getStart();
                      const SCEV* loadStart = loadARE->getStart();

                      // Step: incremento ad ogni iterazione
                      const SCEV* storeStep = storeARE->getStepRecurrence(*SE);
                      const SCEV* loadStep = loadARE->getStepRecurrence(*SE);
                      
                      outs() << "Inizio store: " << *storeStart << "\n";
                      outs() << "Inizio load: " << *loadStart << "\n";

                      // CONTROLLO DIPENDENZA NEGATIVA:
                      // Se hanno lo stesso step, gli indici crescono allo stesso ritmo
                      if(storeStep == loadStep) {
                        // Calcolo la differenza tra i punti di partenza
                        const SCEV* diff = SE->getMinusSCEV(storeStart,loadStart);

                        // Se la differenza è una costante, posso valutarla
                        if(const SCEVConstant * diffConst = dyn_cast<SCEVConstant>(diff)) {
                          int64_t diffValue = diffConst->getAPInt().getSExtValue();
                          outs() << "Differenza tra gli start: " << diffValue << "\n";
                          
                          // DIPENDENZA NEGATIVA: store_start < load_start
                          // Significa che L2 legge da posizioni che L1 scriverà dopo
                          // Questo violerebbe la semantica se i loop fossero fusi
                          if(diffValue < 0) {
                            outs() << "Abbiamo una dipendenza negativa tra i due loop!" << "\n";
                            return true; // Dipendenza trovata = fusione non sicura
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    
      return false; // Nessuna dipendenza negativa trovata
    }

    /**
     * FUSIONE DEI LOOP
     * Implementa la trasformazione vera e propria unendo i due loop
     * Modifica il CFG per creare un singolo loop che esegue entrambi i corpi
     */
    void fuseLoops(Loop* L1, Loop* L2) {
      
      // STEP 1: UNIFICAZIONE DELLE INDUCTION VARIABLES
      // Ogni loop ha una variabile di controllo (induction variable)
      // Sostituisco tutte le occorrenze di IV2 con IV1 per usare un solo contatore
      PHINode* IV1 = L1->getCanonicalInductionVariable();
      PHINode* IV2 = L2->getCanonicalInductionVariable();
      IV2->replaceAllUsesWith(IV1); // LLVM API per sostituire tutti gli usi

      // STEP 2: IDENTIFICAZIONE DEI BLOCCHI CHIAVE
      // Mappo i componenti strutturali di entrambi i loop
      
      // Componenti del primo loop:
      BasicBlock* Exit1 = L1->getExitBlock();        // Blocco di uscita
      BasicBlock* Header1 = L1->getHeader();         // Blocco di ingresso/controllo
      BasicBlock* Latch1 = L1->getLoopLatch();       // Blocco che chiude il loop
      BasicBlock* BodyLast1 = Latch1->getSinglePredecessor(); // Ultimo blocco del corpo
      
      // Componenti del secondo loop:
      BasicBlock* Exit2 = L2->getExitBlock();
      BasicBlock* Latch2 = L2->getLoopLatch();
      BasicBlock* Header2 = L2->getHeader();
      BasicBlock* BodyLast2 = Latch2->getSinglePredecessor();
      BasicBlock* PreHead2 = L2->getLoopPreheader(); // Blocco prima dell'header
      
      // IDENTIFICAZIONE DEL PRIMO BLOCCO DEL CORPO DI L2:
      // Itero sui successori dell'header del secondo loop
      // Il successore che non è l'uscita è il primo blocco del corpo
      BasicBlock* BodyFirst2;
      for(auto succ = succ_begin(Header2); succ != succ_end(Header2); succ++) {
        BasicBlock* succBB = *succ;
        if(succBB != Exit2) {
          BodyFirst2 = succBB; // Primo blocco del corpo di L2
        }
      }

      
      outs() << "Inizio modifica dei branch per la fusione dei loop\n";

      // STEP 3: MODIFICA DEI BRANCH - RICONNESSIONE DEL CFG
      
      // MODIFICA 1: Header del primo loop
      // L'header di L1 ora deve uscire direttamente quando la condizione è falsa
      // invece di passare attraverso L2
      BranchInst* brHeader1 = dyn_cast<BranchInst>(Header1->getTerminator());
      outs() << "Analizzo terminatore di Header1: " << Header1->getName() << "\n\n";
      if(brHeader1->isConditional()){
        outs() << "Header1 ha un branch condizionale\n";
        outs() << "-----------------------------------------" << "\n\n";
        
        // Sostituisco i riferimenti a PreHead2 con Exit2
        // Così quando la condizione del loop è falsa, usciamo completamente
        if(brHeader1->getSuccessor(0) == PreHead2) {
          outs() << "Successore 0 di Header1 è PreHead2, lo sostituisco con Exit2\n";
          outs() << "-----------------------------------------" << "\n\n";
          brHeader1->setSuccessor(0, Exit2);
        }
        if(brHeader1->getSuccessor(1) == PreHead2) {
          outs() << "Successore 1 di Header1 è PreHead2, lo sostituisco con Exit2\n";
          outs() << "-----------------------------------------" << "\n\n";
          brHeader1->setSuccessor(1, Exit2);
        }
      }

      // MODIFICA 2: Ultimo blocco del corpo di L1
      // Invece di tornare al latch di L1, deve passare al corpo di L2
      BranchInst* brBody1 = dyn_cast<BranchInst>(BodyLast1->getTerminator());
      outs() << "Analizzo terminatore di BodyLast1: " << BodyLast1->getName() << "\n";
      outs() << "-----------------------------------------" << "\n\n";
      if(!brBody1->isConditional()){
        outs() << "BodyLast1 ha un branch non condizionale\n";
        outs() << "-----------------------------------------" << "\n\n";
        if(brBody1->getSuccessor(0) == Latch1) {
          outs() << "Successore 0 di BodyLast1 è Latch1, lo sostituisco con BodyFirst2\n";
          outs() << "-----------------------------------------" << "\n\n";
          brBody1->setSuccessor(0, BodyFirst2); // Collegamento sequenziale dei corpi
        }
      }

      // MODIFICA 3: Ultimo blocco del corpo di L2
      // Dopo aver eseguito L2, torniamo al latch di L1 per la prossima iterazione
      BranchInst* brBody2 = dyn_cast<BranchInst>(BodyLast2->getTerminator());
      outs() << "Analizzo terminatore di BodyLast2: " << BodyLast2->getName() << "\n";
      outs() << "-----------------------------------------" << "\n\n";
      if(!brBody2->isConditional()){
        outs() << "BodyLast2 ha un branch non condizionale\n";
        outs() << "-----------------------------------------" << "\n\n";
        if(brBody2->getSuccessor(0) == Latch2) {
          outs() << "Successore 0 di BodyLast2 è Latch2, lo sostituisco con Latch1\n";
          outs() << "-----------------------------------------" << "\n\n";
          brBody2->setSuccessor(0, Latch1); // Chiusura del loop fuso
        }
      }

      // MODIFICA 4: Header del secondo loop (ora inutilizzato)
      // L'header di L2 non sarà più raggiunto, ma per correttezza del CFG
      // impostiamo tutti i suoi successori al suo latch
      BranchInst* brHeader2 = dyn_cast<BranchInst>(Header2->getTerminator());
      outs() << "Analizzo terminatore di Header1 (per Header2): " << Header2->getName() << "\n";
      outs() << "-----------------------------------------" << "\n\n";
      if(brHeader2->isConditional()){
        outs() << "Header1 ha un branch condizionale (per Header2), imposto entrambi i successori a Latch2\n";
        outs() << "-----------------------------------------" << "\n\n";
        brHeader2->setSuccessor(0, Latch2);
        brHeader2->setSuccessor(1, Latch2);
      }

      outs() << "Modifica dei branch completata\n";
      outs() << "-----------------------------------------" << "\n\n";
      
      return;
    }

    /**
     * MAIN ENTRY POINT DEL PASS
     * Punto di ingresso principale: coordina tutte le analisi e la trasformazione
     * Implementa la pipeline completa di controlli per la fusione sicura
     */
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM)
    {
      // STEP 1: ACQUISIZIONE DELLE ANALISI NECESSARIE
      // Il sistema di analisi di LLVM fornisce informazioni sui loop e dominanza
      LoopInfo &LI = AM.getResult<LoopAnalysis>(F);               // Informazioni sui loop
      DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F); // Albero di dominanza
      PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F); // Post-dominanza
      ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);       // Analisi matematica
      DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);             // Analisi dipendenze

      // STEP 2: IDENTIFICAZIONE DEI LOOP DA FONDERE
      // Assumiamo che ci siano esattamente due loop top-level nella funzione
      std::vector<Loop*> Loops = LI.getTopLevelLoops();
      if(Loops.size() < 2) {
        outs() << "Non ci sono abbastanza loop\n";
        return PreservedAnalyses::all(); // Nessuna modifica se meno di 2 loop
      }
      
      // NOTA: L'ordine potrebbe sembrare controintuitivo ma riflette
      // l'ordine di scoperta dei loop da parte di LLVM
      Loop* L1 = Loops[1]; // Primo loop nel codice sorgente
      Loop* L2 = Loops[0]; // Secondo loop nel codice sorgente
      
      // STEP 3: PIPELINE DI CONTROLLI PER LA FUSIONE SICURA
      
      outs() << "-----------------------------------------" << "\n";
      outs() << "| INIZIO CONTROLLO DI ADIACENZA          |" << "\n";
      outs() << "-----------------------------------------" << "\n";
      // Test 1: I loop devono essere fisicamente adiacenti nel CFG
      if(areAdjacent(L1, L2)) {
        outs() << "I loop sono adiacenti\n \n";
      } else {
        outs() << "I loop non sono adiacenti\n \n";
        // In un'implementazione completa, questo dovrebbe bloccare la fusione
      }

      outs() << "-----------------------------------------" << "\n";
      outs() << "| INIZIO CONTROLLO DI CFG EQUIVALENZA    |" << "\n";
      outs() << "-----------------------------------------" << "\n";
      // Test 2: I loop devono avere struttura di controllo equivalente
      if(areCFGEquivalent(L1,L2, DT, PDT)) {
        outs() << "I loop sono CFG equivalenti\n\n";
      } else {
        outs() << "I loop non sono CFG equivalenti\n\n";
        // CFG diversi = semantica diversa = fusione non sicura
      } 

      outs() << "-----------------------------------------" << "\n";
      outs() << "| INIZIO CONTROLLO SUL NUMERO DI ITERAZIONI |" << "\n";
      outs() << "-----------------------------------------" << "\n";
      // Test 3: Stesso numero di iterazioni per preservare la semantica
      if(sameIterationNumber(L1,L2,&SE)){
        outs() << "I loop hanno lo stesso numero di iterazioni\n\n";
      } else {
        outs() << "I loop non hanno lo stesso numero di iterazioni\n\n";
        // Numero diverso = comportamento diverso dopo la fusione
      }

      outs() << "-----------------------------------------" << "\n";
      outs() << "| INIZIO CONTROLLO SULLE DIPENDENZE A DISTANZA NEGATIVA |" << "\n";
      outs() << "-----------------------------------------" << "\n";
      // Test 4: Assenza di dipendenze che violerebbero l'ordine di esecuzione
      if(hasDependence(L1, L2, &DI, &SE)) {
        outs() << "I loop hanno dipendenze\n\n";
        // Dipendenze negative = risultato diverso dopo la fusione
      } else {
        outs() << "I loop non hanno dipendenze\n\n";
        // Nessuna dipendenza = fusione sicura dal punto di vista dei dati
      }

      outs() << "-----------------------------------------" << "\n";

      // STEP 4: ESECUZIONE DELLA TRASFORMAZIONE
      outs() << "-----------------------------------------" << "\n";
      outs() << "| INIZIO FUSIONE DEI LOOP                |" << "\n";
      outs() << "-----------------------------------------" << "\n\n";

      // In questa implementazione dimostrativa, eseguiamo sempre la fusione
      // Un'implementazione di produzione dovrebbe fondere solo se tutti i test passano
      fuseLoops(L1, L2);
      outs() << "-----------------------------------------" << "\n";
      outs() << "| Fusione completata                    |\n";
      outs() << "-----------------------------------------" << "\n";
      
      // STEP 5: NOTIFICA DELLE ANALISI PRESERVATE
      // Indichiamo a LLVM che tutte le analisi sono ancora valide
      // (in realtà alcune potrebbero essere invalidate dalla trasformazione)
      return PreservedAnalyses::all();
    }
  
    // Questa funzione forza l'esecuzione del pass anche con -O0
    // Normalmente LLVM salta le ottimizzazioni quando optnone è impostato
    static bool isRequired() { return true; }
  };  
} // namespace

//-----------------------------------------------------------------------------
// New PM Registration - Registrazione nel sistema di Pass di LLVM
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getTestPassPluginInfo()
{
  return {LLVM_PLUGIN_API_VERSION, "LoopFusion1", LLVM_VERSION_STRING,
          [](PassBuilder &PB)
          {
            // Registra il callback per riconoscere il nome "loop-fusion1"
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                  if (Name == "loop-fusion1") 
                  {
                    FPM.addPass(LoopFusion1()); // Aggiunge il nostro pass alla pipeline
                    return true;
                  }
                  return false;
                });
          }};

}

// Interfaccia C per il caricamento dinamico del plugin
// Questa funzione è chiamata da 'opt' quando carica il plugin
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo 
llvmGetPassPluginInfo()
{
  return getTestPassPluginInfo();
}
