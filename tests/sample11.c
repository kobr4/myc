int a;
int * c;
/* zouz */
int func(int * b, int * d) {
    
    return *b + 3 + *d;
}


int main(int argc, char **argv) {
    //toto titi
    a = 11;
    c = &a;
    return func(&a, c);
}