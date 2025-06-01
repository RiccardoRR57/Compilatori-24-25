/*
int main()
{
    int A[12];
    int a = 0;
    int b = 0;
    int c = 12;
    
    if(c > -12) {
        do{
            A[a] = a*4;
            a++;
        } while ( a < 12);
    }

    if(c > -12) {
        do{
            c = A[b];
            b++;
        } while ( b < 12);
    }
}
*/

int main()
{
    int A[10];
    int b = 0;
    
    //in questa coppia non sarà possibile la loop fusion, in quanto il secondo loop ha una dipendenza negativa
    for (int i = 0; i < 5; i++) {
        A[i] = i*2;
    }
    for(int i = 0; i < 5; i++) {
        b = A[i+1];
    }
    
    //in questa coppia sarà possibile la loop fusion, in quanto tutte le condizioni hanno esito positivo
    for (int i = 0; i < 5; i++) {
        A[i] = i*2;
    }
    for(int i = 0; i < 5; i++) {
        b = A[i];
    }
}