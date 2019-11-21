char msg[] = "HELLO";

int strlens(char * a) {
    int i = 0;
    while(a[i] != 0) 
        i++;
    
    return i;
}

int main(int argc, char **argv) {

    return strlens(msg);
}