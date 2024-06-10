#include <psutil-cpp/cpu.hpp>
#include <psutil-cpp/mem.hpp>
#include <iostream>

int main()
{
	std::cout << virtual_memory().value() << std::endl; // Returns svmem struct
	std::cout << swap_memory().value() << std::endl;	// Returns sswap struct
	std::cout << cpu_times().value()[0] << std::endl;	// Returns std::vector with only one element (scputimes struct)
	std::cout << cpu_times(true).value() << std::endl;	// Returns std::vector of scputimes structs
	std::cout << cpu_count() << std::endl;				// Returns number cpus. When logical is false (cpu_count(false)) currently doesn't work!

	return 0;
}
