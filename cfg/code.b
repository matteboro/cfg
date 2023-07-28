data t {
  int :: b,
  string :: f,
  int arr[10]:: a
}

data s {
  t arr[10]:: a,
  string :: b
}

s :: v;
v.a[3].a[6] = 10;
int :: a = 0;
a = 10;
int :: b;
b = a + 10;