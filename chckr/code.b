data struct1 {
  int arr[10] :: att1,
  string :: att2
}

data struct2 {
  struct1 :: s1,
  int :: val
}

data struct3 {
  struct2 :: s2,
  struct2 arr[10] :: s2_arr,
  string :: name,
  struct1 :: s1
}

func exp (int :: val) {
  int :: ret_val = val * val;
  return ret_val;
}

func print_int (int :: i) {
  # this function should print an integer to the screen
}

func main (int :: n_args, int arr[10] :: args) {
  int :: i = 0;
  while (i < n_args) {
    int :: garbage = print(args[i]);
    i = i + 1;
  }
  int :: f;
  if (j < 42) {
    string :: err = "error";
    f =  < err >;
  } elif (j > 56) {
    string :: corr = "correct";
    f =  < corr >;
  } else {
    string :: dknow = "dont know";
    f =  < dknow >;
  }
  print_int(f);
  return f + j * i;
}

main(0, null);
print_int(100);
exp(0);
# print_string("heyy\n");
int :: i1 = 10;
string :: f2 = "Hello, world";
struct3 :: f3;
{
  int :: s2 = 10;
  string :: g4 = "Hello, world";
  struct1 :: t8;
}
int :: t8;
t8 = 10;
# f3 = 10;
f3.s2_arr[10].s1.att1[10] = 10;