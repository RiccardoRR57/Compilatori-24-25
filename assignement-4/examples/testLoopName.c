int main()
{
    int A[20];
    int a = 0;
    int b = 0;
    int c = 0;
    
    for (int i = 0; i < 10; i++) {
        A[i] = i*2;
    }
    for(int i = 0; i < 10; i++) {
        b = A[i];
        for(int j = 0; j < 10; j++) {
            c = A[j];
        }
    }
    for(int i = 0; i < 20; i++) {
        c = A[i];
    }
    for(int i = 0; i < 5; i++) {
        A[i] = A[i+1];
    }
}