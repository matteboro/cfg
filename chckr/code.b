data struct1 {
  int arr[10]:: att1,
  string :: att2,
  int ptr :: p
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

func int :: exp(int :: val) {
  int :: ret_val = val * val;
  return ret_val;
}

func int :: print_int(int :: i) {
  # this function should print an integer to the screen
  return 0;
}

func string :: print_int2(string :: s) {
  return "hello";
}

func int :: main(int :: n_args, int arr[10] :: args) {
  int :: i = 0;
  while (i < n_args) {
    int :: garbage = print_int(args[i]);
    i = i + 1;
  }
  int :: f;
  int :: j;
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

func string :: int_to_string(int :: val) {

}

func int :: string_to_int(string::s) {

}

func int :: foo (int :: i1, int :: i2, int :: i3) {

}

int arr[10] :: args;
main(0, args);
int :: t;
foo(t, t, t);
int_to_string(t);
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
f3.s2_arr[10].s1.att1[9] = 10;
int arr[5] :: my_arr = [1, 2, 3, < int_to_string(< "hello, " | "world" >) >, 5];
int :: v = string_to_int("10");
int :: my_var = 0;

if (< int_to_string(t8) >) {
  int :: dodo;
}

while (my_var < 10) {
  int :: some_var = 90;
  string :: some_str = int_to_string(some_var);
  my_var = my_var + 1;
}

t = 10 * 10 + 10;
t = 10 + 10;
t = 10 - 10;
t = 10 / 10;
t = 10 < 10;
t = 10 > 10;
t = 10 == 10;
t = -10;
t = 2 * < "hello" > <= < "hello" | "world" >;
string :: s;
s = "hello" | ", world!";

string arr[5] :: s_arr;
s_arr[0] = "hello";

int strong ptr :: p = create int[10 * 10];
p = create int;

global int :: global_i = 10;



# object deref errors tests

# struct1 :: s_1;
# struct3 :: s_3;

# type of index expression is not integer
# s_1.p["hello"] = 10;

# type of index expression is not valid
# s_1.p[10 * "hello"] = 10;

# should pr should not be array deref
# s_3.s2_arr.val = 10;

# should not be a array deref
# s_1.att2[10] = "hello";

# should not be single element deref
# s_1.[att1] = "hello";

# does no tlink to attribute
# s_1.f = 10;

# var not exists
# s_2 = 10;
