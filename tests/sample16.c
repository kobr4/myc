

int main(int argc, char **argv) {
    char t[100];

    for (int i = 0;i < 100;i++) {
        t[i] = i + 1;
    }

    char * z = &t[50];

    return *z;
}