char a[] = "HELLO";
char b[] = "HELLA";

int my_strlen(char * a) {
    int i = 0;
    while(a[i] != 0) 
        i++;
    
    return i;
}

int my_strcmp(char * a, char * b) {

    int len_a = my_strlen(a);
    int len_b = my_strlen(b);

    if (len_b != len_a) {
        return 2;
    }

    for(int i = 0;i < len_a;i++) {   
        if (a[i] != b[i]) {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char **argv) {

    return my_strcmp(a, b);
}