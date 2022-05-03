#include <iostream>
int get_local_id(int);
int main() {
// The following code will produce a warning because this ID-dependent
// variable is used in a loop condition statement.
int ThreadID = get_local_id(0);

while (ThreadID > 0) {}

// The following loop will produce a warning because the loop condition
// statement depends on an ID-dependent variable.
for (int i = 0; i < ThreadID; ++i) {
  std::cout << i << std::endl;
}

// The following loop will not produce a warning, because the ID-dependent
// variable is not used in the loop condition statement.
for (int i = 0; i < 100; ++i) {
  std::cout << ThreadID << std::endl;
}
}