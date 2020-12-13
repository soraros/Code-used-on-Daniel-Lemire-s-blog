#include "simdjson.h"
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <chrono>

using clk = std::chrono::high_resolution_clock;
using time_point = std::chrono::time_point<clk>;
using dur_double = std::chrono::duration<double>;
using std::chrono::duration_cast;

__attribute__((noinline))
double time_it(const char * some_string, size_t length, char *output) {
  size_t new_length{}; // It will receive the minified length.
  auto start = clk::now();
  auto error = simdjson::minify(some_string, length, output, new_length);
  auto duration = clk::now() - start;
  auto elapsed_s = duration_cast<dur_double>(duration).count();
  return length / (1000. * 1000 * 1000.0) /  elapsed_s;
}
int main(int argc, char** argv) {
  const char * filename = "twitter.json";
  if(argc == 1) { filename = argv[0]; }
  std::ifstream ifs(filename);
  if(!ifs) {
      std::cerr << "error reading " << filename << std::endl;
      return EXIT_FAILURE;
  }
  std::string file_content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
  std::unique_ptr<char[]> buffer{new char[file_content.size()]};
  double best_speed = 0;
  for(size_t i = 0; i < 5000; i++) {
      double this_speed = time_it(file_content.data(), file_content.size(), buffer.get());
      if(this_speed > best_speed) { best_speed = this_speed; }
  }
  std::cout << best_speed << " GB/s" << std::endl;

  return EXIT_SUCCESS;
}