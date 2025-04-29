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
     - È una costante
     - Tutti i suoi operandi sono costanti o loop-invariant
     - Non ha effetti collaterali
     - Non dipende da istruzioni all'interno del loop

2. **Code Motion**
   - Le istruzioni identificate come invarianti vengono spostate:
     - Fuori dal loop
     - In un punto dominatore del loop
     - Prima dell'ingresso nel loop

## Utilizzo

Per applicare l'ottimizzazione al codice LLVM IR:

```bash
# Compilazione del pass
mkdir build && cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
make

# Esecuzione dell'ottimizzazione
opt -load-pass-plugin=lib/libAssignement3.so -passes="loop-invariant" input.ll -o output.ll
```

## Test

La cartella `examples/` contiene alcuni file di test per verificare il corretto funzionamento dell'ottimizzazione:

- `Loop2.c`: Esempio base con calcolo costante nel loop
- `Loop.c`: Esempio con variabile globale e funzione di incremento