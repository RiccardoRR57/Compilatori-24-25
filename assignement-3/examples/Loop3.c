#include <stdio.h>

int main()
{
    int x = 10; // Loop invariant
    int y = 5;  // Loop invariant
    int result = 0;
    int invariant = 0;

    for (int i = 0; i < 10; i++)
    {
        if (i < 5)
        {
            // Loop invariant ma non optimizable
            result = i * 2;
            // perché questo basic block non domina
            // l'uscita del loop nel ramo else
            result = invariant + i;
            break;
        }
        invariant = x + y; // Uscita alternativa dal loop

        if(i > 5) {
            int cappellone = invariant + 1; // cappellone fa sì che l'istruzione invariant non muoia
            break;
        }
    }
    

    return result;
}