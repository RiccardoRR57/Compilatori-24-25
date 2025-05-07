# Compilatori 2024-2025 - Assignment 3: Loop Invariant Code Motion

Questa cartella contiene il codice e la documentazione relativi all'ottimizzazione Loop Invariant Code Motion (LICM) implementata in LLVM IR.

## Descrizione

Il Loop Invariant Code Motion è un'ottimizzazione che identifica le istruzioni all'interno di un loop che producono sempre lo stesso risultato ad ogni iterazione (loop-invariant) e le sposta fuori dal loop. Questo permette di:
- Ridurre il numero di istruzioni eseguite all'interno del loop
- Migliorare le prestazioni del codice
- Aumentare la leggibilità del codice ottimizzato

## Implementazione

L'ottimizzazione è implementata attraverso due fasi principali:

1. **Identificazione delle istruzioni invarianti**
   - Un'istruzione è loop-invariant se:
     - É una Binary Operation
     - Tutti i suoi operandi sono costanti 
     - Non dipende da istruzioni all'interno del loop
     - I suoi operandi sono istruzioni loop invariant a loro volta

2. **Controllo possibile Code Motion**
  - Un'istruzione è candidata alla Code Motion se:
    - É loop-invariant
    - Domina tutte le uscite del loop oppure è morta nelle uscite che non domina


3. **Code Motion**
   - Le istruzioni identificate come invarianti vengono spostate:
     - Nel Pre header del loop

La fase di Code Motion prevede lo spostamento delle istruzioni candidate nell'ordine corretto mantenendo il control flow del programma inalterato. 
Per assicurarci che questo sia possibile andiamo a iterare sulle istruzioni candidate alla Code Motion, e se questa ha come operandi delle istruzioni interne al loop andremo a richiamare su di essi ricorsivamente la funzione che si occupa dello spostamento delle istruzioni. Per far sì che inoltre non si anilizzino più volte le stesse istruzioni abbiamo aggiunto un controllo preliminare, dove se l'istruzione analizzata è già stata spostata fuori dal loop terminiamo la funzione.

## Utilizzo

Per applicare l'ottimizzazione al codice LLVM IR:

```bash
# Compilazione del pass
mkdir build && cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
make

# Esecuzione dell'ottimizzazione
cd ../examples
opt -load-pass-plugin=../build/libAssignement3.so -passes="loop-invariant" input.ll -o output.ll
```

## Test

La cartella `examples/` contiene alcuni file di test per verificare il corretto funzionamento dell'ottimizzazione:

