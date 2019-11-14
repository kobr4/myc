int main(int argc, char**argv) {
    int a = 0;
    int * b = &a;
    *b = 10;
    return *b;
}