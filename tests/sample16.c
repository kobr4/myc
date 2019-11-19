

int main(int argc, char **argv) {
    char t[100];

    for (int i = 1;i < 101;i++) {
        t[i-1] = i + 1;
    }

    char * z = &t[50];

    return *z;
}