# Compilatori 2024-2025 - Assignment 1

Questa cartella contiene il codice e i risultati relativi all'Assignment 1 del corso di Compilatori.

## Prerequisiti

- LLVM installato sul sistema
- Clang compiler
- Make (opzionale, ma consigliato)

## Esecuzione da linea di comando

### 1. Compilazione del file Assignement1.cpp

```bash
# Crea ed entra nella cartella build
mkdir build
cd build

# Genera il Makefile
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..

# Compilazione effettiva
make
```


### 1. Compilazione del codice sorgente in LLVM IR

```bash
# Entra nella cartella examples
cd ../examples
# Genera il codice LLVM IR non ottimizzato
clang -S -emit-llvm mio_programma.c -o mio_programma.ll
```

### 2. Applicazione delle singole ottimizzazioni

```bash
# Entra nella cartella examples dove è presente il file LLVM IR
cd examples

# Esegui l'ottimizzazione di identità algebrica
opt -load-pass-plugin=../build/libAssignement1.so -passes="algebraic-identity" mio_programma.ll -So mio_programma_ottimizzato.ll
# Alternativa per architetture ARM (Apple Silicon)
opt -load-pass-plugin=../build/libAssignement1.dylib -passes="algebraic-identity" mio_programma.ll -So mio_programma_ottimizzato.ll

# Esegui l'ottimizzazione di strength reduction
opt -load-pass-plugin=../build/libAssignement1.so -passes="strength-reduction" mio_programma.ll -So mio_programma_ottimizzato.ll
# Alternativa per architetture ARM (Apple Silicon)
opt -load-pass-plugin=../build/libAssignement1.dylib -passes="strength-reduction" mio_programma.ll -So mio_programma_ottimizzato.ll

# Esegui l'ottimizzazione di multi instruction optimization
opt -load-pass-plugin=../build/libAssignement1.so -passes="multi-instruction" mio_programma.ll -So mio_programma_ottimizzato.ll
# Alternativa per architetture ARM (Apple Silicon)
opt -load-pass-plugin=../build/libAssignement1.dylib -passes="multi-instruction" mio_programma.ll -So mio_programma_ottimizzato.ll

# Esegui le tre ottimizzazioni contemporaneamente
opt -load-pass-plugin=../build/libAssignement1.so -passes="all" mio_programma.ll -So mio_programma_ottimizzato.ll
# Alternativa per architetture ARM (Apple Silicon)
opt -load-pass-plugin=../build/libAssignement1.dylib -passes="all" mio_programma.ll -So mio_programma_ottimizzato.ll
```

## Descrizione delle ottimizzazioni

### Algebraic Identity
Identifica e semplifica le operazioni aritmetiche con elementi neutri, riducendo la complessità computazionale:

#### Operazioni con addizione e sottrazione
- **Addizione per zero**: Trasforma `x + 0` o `0 + x` in `x` 
- **Sottrazione per zero**: Trasforma `x - 0` in `x`

#### Operazioni con moltiplicazione e divisione
- **Moltiplicazione per uno**: Trasforma `x * 1` o `1 * x` in `x`
- **Divisione per uno**: Trasforma `x / 1` in `x`

Queste trasformazioni preservano il risultato matematico originale mentre rimuovono calcoli ridondanti, migliorando l'efficienza senza alterare la semantica del programma.

### Strength Reduction 
Sostituisce operazioni costose con operazioni equivalenti ma più efficienti:

#### Moltiplicazione
- **Moltiplicazione per potenze di 2**: Sostituisce `x * 2^n` con l'operazione di shift `x << n`, che è computazionalmente più efficiente.
- **Moltiplicazione per costanti vicine a potenze di 2**:
  - Per costanti del tipo `2^n - 1`: Sostituisce `x * (2^n - 1)` con `(x << n) - x`
  - Per costanti del tipo `2^n + 1`: Sostituisce `x * (2^n + 1)` con `(x << n) + x`

#### Divisione
- **Divisione per potenze di 2**: Sostituisce `x / 2^n` con l'operazione di shift `x >> n` per tipi unsigned, o con una sequenza appropriata di istruzioni per tipi signed (per gestire correttamente l'arrotondamento).

### Multi Instruction
Ottimizza sequenze di istruzioni per rimuovere ridondanze e operazioni inutili:

#### Fusione di operazioni
- **Addizioni e sottrazioni successive ridondanti**:
  - Per sequenze del tipo `a = b + n`, `c = a - n`: Elimina la seconda istruzione e sostituisce `c` con `b`
  - Per sequenze del tipo `a = b - n`, `c = a + n`: Elimina la seconda istruzione e sostituisce `c` con `b`
