data struct1 {
  int :: att1,
  string :: att2,
  int arr[10] :: att3,
  struct2 :: att4
}

data struct2 {
  string :: att1,
  struct1 :: att2,
  struct2 :: att3
}

data struct3 {
  struct4 :: att1
}