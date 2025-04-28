
int test() {
    int x = 10;
    int y = 20;
    int sum = 0;
    
    for(int i = 0; i < 100; i++) {
        int const_calc = x * y;    // Questa sarà loop invariant
        sum += const_calc + i;     // Questa no
    }
    return sum;
}