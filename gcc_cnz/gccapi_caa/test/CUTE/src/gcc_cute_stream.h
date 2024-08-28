#include "cute_suite.h"

#include "aes_gcc_stream.h"

class AES_GCC_Stream;

class AES_GCC_CUTE_Stream
{
	public:
	static cute::suite make_suite_gcc_cute_stream();
	static void StreamFunctionality();
};
