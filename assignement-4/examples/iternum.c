int main()
{
    int A[10];
    int a = 0;
    int b = 0;
    do
    {
        A[a] = 2*a;
        a++;
    } while (a < 10);
    int d = 0;
    do
    {
        b =A[d];
        d++;
    } while (d < 10);
}