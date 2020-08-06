extern void __VERIFIER_error() __attribute__ ((__noreturn__));
extern int __VERIFIER_nondet_int();
extern short __VERIFIER_nondet_short();
void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: __VERIFIER_error();
  }
  return;
}

struct node
{
  const int id,id1,id2;
  double value;
  char str;
}

int main(void) {
  struct node n;
  unsigned int x = 0;
  unsigned int y = 1;
  while (__VERIFIER_nondet_int()) {
    x += 3;
  }
  __VERIFIER_assert(!(x % 2));
  while (__VERIFIER_nondet_short()) {
    y += 2;
  }
  __VERIFIER_assert(y % 2);
  return 0;
}
