int main() {
    int a = 0;
    
    for (int i = 0;i < 3;i++) {
        a = a+1;
    }

    for (int i = 3;i > 0;i--) {
        a = a+1;
    }

    for (int i = 0;i == 0;i++) {
        a = a+1;
    }   

    for (int i = 0;i < 3;i++)
        a = a+1;

    return a;
}