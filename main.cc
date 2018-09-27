#include "Mesh.hpp"

extern "C" {

int
main_default(int argc, char **argv)
{

	return (0);
}

int main(int argc, char **argv) __attribute__((weak, alias("main_default")));
}
