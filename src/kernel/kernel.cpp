/*  ===== Date Created: 11 September, 2020 =====  */

#include "system.hpp"
#include "display.hpp"

extern "C" void start_kernel()
{
	print("Hello, it's FancyKillerPanda here! ", 0x9f);
	print("This is a super long line. It should hopefully advance to the next one.", 0x04);
	while (true);
}
