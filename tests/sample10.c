int a;

int main(int argc, char**argv) {
    a = 0;
    int * b = &a;
    *b = 10;
    return *b;
}