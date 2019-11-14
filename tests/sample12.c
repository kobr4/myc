int func(int a, int c, int d) {
  int b = a + c + d;
  return b;
}

int main(int argc, char **argv) {
  int b = 0;
  if (b < 2) 
    b = 1;
  else 
    b = 2;
  
  int a = 77 + func(3, 4, 11) + b;
  return a;
}