#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <random>

// Expected distance of a 1-d random walk after N steps.
//
// http://math.stackexchange.com/questions/103142/expected-value-of-random-walk
// http://mathworld.wolfram.com/RandomWalk1-Dimensional.html
// 
// First time I use <random>. Comments are welcome.
// nelsoneci@gmail.com.

using namespace std;

class RandomDigits {
 public:
  RandomDigits() {
    std::random_device rd;
    gen.reset(new std::mt19937(rd()));
    uniform_0_1.reset(new std::uniform_int_distribution<>(0, 1));
  }
  int next() {
    return (*uniform_0_1.get())(*gen.get());
  }
 private:
   unique_ptr<std::mt19937> gen;
   unique_ptr<std::uniform_int_distribution<>> uniform_0_1;
};

int random_walk_distance(RandomDigits* rng, int N) {
  int current = 0;
  while (N--) {
    current += rng->next() ? 1 : -1;
  }
  return abs(current); // Distance from 0.
}

double theoretical_distance(double N) {
  return sqrt((2 * N) / M_PI);
}

int main(int, char **) {
  RandomDigits rnd;
  const int N_SAMPLES = 20000;
  const int WALK_LEN = 1000;
  double dist  = 0;
  for (int i = 0; i < N_SAMPLES; ++i) {
    dist += random_walk_distance(&rnd, WALK_LEN);
  }
  dist /= N_SAMPLES;
  std::cout << "Theoretical average distance: " << theoretical_distance(WALK_LEN) << endl;
  std::cout << "Empirical average distance: " << dist << endl;
  return EXIT_SUCCESS;
}
