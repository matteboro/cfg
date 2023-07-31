data t {
  int :: b,
  string :: f,
  int arr[10]:: a
}

data s {
  t arr[10]:: a,
  string :: b,
  t ptr :: t_ptr
}

# int ptr :: p;
# int :: a;

# a = 10;
# b[5] = 4;
# [p] = 5;
# p[10] = 10;

int arr[10] :: b;
int :: c;

s :: a;
# a.a[2].b = 10;
# a.a[10].a[2] = 2;

# a.t_ptr[10 * c + b[4]].b = 10;

 t arr[10] :: t_arr;
 t_arr[4].b = 10;