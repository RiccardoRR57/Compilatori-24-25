


const int N = 100;
int a[N], b[N], c[N];
int i1 = 1;
int i2 = 1;
int guard = 5;


int main()
{
    if(guard < 4){
        do {
            i1 = i1+1;
        } while (i1 < N);
    }

    if(guard < 4){
        do {
            i2 = i2+1;
        } while (i2 < N);
    }
    

    return 0;
}
