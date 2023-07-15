data struct1 {
  int :: att1,
  string :: att2
}

data struct2 {
  struct1 :: s1,
  int :: val
}

data struct3 {
  struct2 :: s2,
  string :: name,
  struct1 :: s1
  # animal arr[10] :: arr_an
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
int :: i1 = 10;
string :: f2 = "Hello, world";
struct1 :: f3;
{
  int :: s2 = 10;
  string :: g4 = "Hello, world";
  struct1 :: t8;
}

int arr[6] :: t8;