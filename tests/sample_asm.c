int func(int a) {
    asm {
        jsr -552(a6)
    }
    return a;
}

int main(int argc, char **argv) {
    return 0;
}