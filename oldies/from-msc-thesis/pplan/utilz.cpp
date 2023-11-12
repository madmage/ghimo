#include "utilz.h"
#include "stdio.h"

#include <fstream>

// FIXME, doesn't the standard library include a random(int) function yet?
int myrandom(unsigned int Max)
{
	return (Max ? (unsigned int) (std::rand() % Max) : 0);
}
