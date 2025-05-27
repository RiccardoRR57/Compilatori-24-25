int main()
{
    int A[11];
    int a = 0;
    int b = 0;
    
    for (int i = 0; i < 10; i++) {
        A[i] = i*2;
    }
    for(int i = 0; i < 10; i++) {
        b = A[i+1];
    }
}