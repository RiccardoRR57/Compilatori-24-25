

int main() {
    int a = 10;
    int b = 5;
    int result = 0;
    int final_value = 0;
    int complex_calc;
    
    // Loop con uscite multiple e istruzioni loop invariant che non
    // dominano tutte le uscite
    for (int i = 0; i < 100; i++) {
        if (i < 50) {
            // Early exit path
            break; // Prima uscita
        }
        
        // Questa è loop invariant ma viene eseguita solo se i >= 50
        // Quindi non domina tutti i percorsi di uscita
        complex_calc = a * b + 42; // Loop invariant
        
        result += complex_calc;
        
        if (i > 80) {
            // Seconda uscita
            final_value = complex_calc; // Uso di complex_calc dopo il loop
            break;
        }
    }

    
    
    // Altro esempio con controllo condizionale
    int x = 20;
    int y = 7;
    int z = 0;
    
    for (int j = 0; j < 50; j++) {
        if (j % 10 == 0) {
            // Questa è una operazione loop invariant ma eseguita solo su alcune iterazioni
            // Non può essere spostata fuori perché il risultato viene usato dopo il loop
            int special_value = x * y - 15; // Loop invariant
            
            // Uscita precoce che usa special_value
            if (j > 30) {
                z = special_value; // Uso di special_value in un'uscita
                break;
            }
            
            // Usiamo il valore anche nel percorso normale
            result += special_value;
        }
    }
    
    return 0;
}