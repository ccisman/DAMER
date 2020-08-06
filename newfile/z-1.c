int nondet_num_short=101;
int nondet_num_int=101;
void __VERIFIER_error()
		{
			return;
		}

int __VERIFIER_nondet_int()
		{
			nondet_num_int = nondet_num_int - 1;
			return nondet_num_int;
		}

short __VERIFIER_nondet_short()
		{
			nondet_num_short = nondet_num_short - 1;
			return nondet_num_short;
		}

void __VERIFIER_assert(int cond) {
  if (!(cond)) {
     __VERIFIER_error();
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
    x =x + 3;
  }
  __VERIFIER_assert(!(x % 2));
  while (__VERIFIER_nondet_short()) {
    y += 2;
  }
  __VERIFIER_assert(y % 2);
  return 0;
}
