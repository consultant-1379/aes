#include "cute_suite.h"
#include "aes_gcc_message_queue.h"

class GCC_Cute_MessageQueue
{

public:
static cute::suite make_suite_gcc_cute_message_queue();
static void vPutq();
static void vGetq();
static void vPrioritize();
static void vGetSize();
static AES_GCC_MessageQueue<std::string> message;
};
