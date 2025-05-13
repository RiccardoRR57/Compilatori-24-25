# Compilatori 2024-2025 - Assignment 4: Loop Fusion

Questa cartella contiene il codice e la documentazione relativi all'ottimizzazione Loop Fusion implementata in LLVM IR.

## Descrizione

Loop Fusion (o Loop Combining) è un'ottimizzazione che mira a combinare due o più loop adiacenti in un singolo loop. Questa trasformazione è vantaggiosa quando i loop combinati:

- Hanno lo stesso numero di iterazioni o possono essere adattati per averlo.
- Non hanno dipendenze di dati che impediscono la fusione.
- Eseguono operazioni che possono beneficiare della località dei dati migliorata nel loop combinato.

I vantaggi principali di Loop Fusion includono:

- Riduzione dell'overhead del loop (meno inizializzazioni, incrementi e condizioni di terminazione).
- Miglioramento della località dei dati e riutilizzo della cache.
- Possibilità di esporre ulteriori opportunità di ottimizzazione.

## Implementazione

L'implementazione di Loop Fusion segue generalmente questi passi:

1. **Analisi di Dipendenza:**
   - Verificare che i loop candidati non abbiano dipendenze di dati che impedirebbero la fusione sicura. Questo include controllare dipendenze read-after-write (RAW), write-after-read (WAR) e write-after-write (WAW).

2. **Verifica delle Condizioni di Fusione:**
   - Assicurarsi che i loop abbiano intervalli di iterazione compatibili o che uno possa essere adattato all'altro.
   - Controllare che i loop non contengano istruzioni che causerebbero problemi se combinate (es. chiamate a funzioni con effetti collaterali, istruzioni di I/O).

3. **Fusione dei Loop:**
   - Combinare i corpi dei loop in un singolo loop.
   - Adattare gli indici dei loop e le condizioni di terminazione, se necessario.

4. **Pulizia:**
   - Rimuovere i loop originali.
   - Eseguire ulteriori ottimizzazioni per sfruttare i vantaggi della fusione.

## Utilizzo

Per applicare l'ottimizzazione al codice LLVM IR:

```bash
# Compilazione del pass
mkdir build && cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
make

# Esecuzione dell'ottimizzazione
cd ../examples
opt -load-pass-plugin=../build/libAssignement4.so -passes="loop-fusion" input.ll -o output.ll