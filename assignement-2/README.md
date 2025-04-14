# Compilatori 2024-2025 - Assignment 2: Dataflow Analysis Frameworks

Questa cartella contiene la documentazione relativa ai framework di analisi del flusso dati implementati in LLVM IR.

## Contenuto

Il documento LaTeX presente in questa repository analizza tre importanti framework di dataflow analysis:

### 1. Very Busy Expression Analysis
Un'espressione è  Very Busy in un punto p se, indipendentemente dal percorso preso da p, l'espressione viene usata prima che uno dei suoi operandi venga definito.

### 2. Dominator Analysis
In un CFG diciamo che un nodo X domina un altro nodo Y se il nodo X appare in ogni percorso dal grafo che porta dal blocco ENTRY al blocco Y.

### 3. Constant Propagation
L'obiettivo della Constant Propagation è quello di determinare in quali punti del programma le variabili hanno un valore costante.
L'informazione da calcolare per ogni nodo n del CFG è un insieme di coppie del tipo <variabile, valore costante>. 
Se abbiamo la coppia <x, c> al nodo n, significa che x è garantito avere il valore c ogni volta che n viene raggiunto durante l'esecuzione del programma.


## Note
Il documento per ognuno di questi framework realizzati contiene:
- Definizione
- Dominio dell'analisi
- Direzione dell'analisi
- Funzione di trasferimento
- Meet operator
- Boundary Contition
- Initial interior point


