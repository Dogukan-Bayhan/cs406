#include <algorithm>
#include <vector>
#include <iostream>
#include <chrono>

// If the data are sorted like shown here the program runs about
// 6x faster (on my test machine, with -O2)


int main() {
  // generate data
  const size_t arraySize = 32768;
  std::vector<int> data(arraySize);

  for (unsigned c = 0; c < arraySize; ++c) {
    data[c] = std::rand() % 256;
  }

  auto start = std::chrono::high_resolution_clock::now();

  long long sum = 0;  
  for (unsigned i = 0; i < 10000; ++i) {
    for (unsigned c = 0; c < arraySize; ++c) {
      int t = (data[c] - 128) >> 31;
      sum += ~t & data[c];
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end - start;

  std::cout << "sum = " << sum << std::endl;
  std::cout << "Elapsed time = " << diff.count() << " seconds" << std::endl;
}
