int t[100];

int main(int argc, char **argv) {

    for (int i = 0;i < 100;i++) {
        t[i] = i;
    }

    return t[50] + 1;
}