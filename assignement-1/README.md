# Compilatori 2024-2025 - Assignment 1

Questa cartella contiene il codice e i risultati relativi all'Assignment 1 del corso di Compilatori.

## Prerequisiti

- LLVM installato sul sistema
- Clang compiler
- Make (opzionale, ma consigliato)

## Struttura della directory

```
assignement-1/
├── src/            # Contiene i file sorgente
├── build/          # Directory per i file compilati
├── optimizations/  # Output delle varie fasi di ottimizzazione
└── README.md       # Questo file
```

## Esecuzione da linea di comando

### 1. Compilazione del codice sorgente in LLVM IR

```bash
# Genera il codice LLVM IR non ottimizzato
clang -S -emit-llvm src/mio_programma.c -o build/mio_programma.ll

# Oppure con ottimizzazioni di base
clang -S -emit-llvm -O1 src/mio_programma.c -o build/mio_programma_O1.ll
```

### 2. Applicazione delle singole ottimizzazioni

```bash
# Esegui l'ottimizzazione di analisi della memoria
opt -mem2reg build/mio_programma.ll -S -o optimizations/mem2reg.ll

# Esegui la propagazione delle costanti
opt -constprop build/mio_programma.ll -S -o optimizations/constprop.ll

# Esegui il dead code elimination
opt -dce build/mio_programma.ll -S -o optimizations/dce.ll

# Combina più ottimizzazioni
opt -mem2reg -constprop -dce build/mio_programma.ll -S -o optimizations/combined.ll
```

### 3. Generazione del codice assembly e compilazione finale

```bash
# Genera assembly x86
llc optimizations/combined.ll -o build/mio_programma.s

# Compila in eseguibile
gcc build/mio_programma.s -o build/mio_programma
```

## Descrizione delle ottimizzazioni

### mem2reg
Questa ottimizzazione promuove le allocazioni di memoria nello stack (alloca) a registri nel SSA (Static Single Assignment). Elimina le istruzioni di load e store non necessarie, migliorando significativamente le prestazioni e preparando il terreno per altre ottimizzazioni.

### constprop (Constant Propagation)
Identifica le variabili che contengono valori costanti e sostituisce i riferimenti a queste variabili con i valori costanti corrispondenti. Questo permette di semplificare le espressioni e ridurre il numero di istruzioni.

### dce (Dead Code Elimination)
Rimuove il codice che non influisce sul risultato del programma, come variabili non utilizzate, istruzioni il cui risultato non viene mai letto, o codice irraggiungibile.

### licm (Loop Invariant Code Motion)
Sposta le istruzioni invarianti fuori dai cicli, riducendo il numero di operazioni eseguite durante l'iterazione del ciclo.

### indvars (Induction Variable Simplification)
Semplifica le variabili di induzione nei cicli, riducendo la complessità delle espressioni e migliorando le prestazioni.

## Note aggiuntive

Per confrontare le diverse ottimizzazioni, è possibile utilizzare strumenti come `diff` o editor di testo per visualizzare le differenze tra i vari file IR generati:

```bash
diff -y optimizations/mem2reg.ll