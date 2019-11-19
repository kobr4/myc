int t[100];

int main(int argc, char **argv) {

    for (int i = 0;i < 100;i++) {
        t[i] = i + 1;
    }

    int * z = &t[50];

    return *z;
}