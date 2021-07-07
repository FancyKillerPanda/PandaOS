//  ===== Date Created: 07 July, 2021 ===== 

#define PANDA_OS_MAGIC_STRING "PandaOS Magic!"

// static const u8* pandaOSCorrectString; // Will be set later
extern const u8 pandaOSMagicString[sizeof(PANDA_OS_MAGIC_STRING)];

// Ensures that the entire kernel image was loaded
void check_magic_string()
{
	const u8* pandaOSCorrectString = PANDA_OS_MAGIC_STRING;

	for (u8 i = 0; pandaOSCorrectString[i] != 0; i++)
	{
		if (pandaOSMagicString[i] != pandaOSCorrectString[i])
		{
			log_error("Magic string index %d (%d/'%c') is not correct (should be %c)!",
					  i, (u32) pandaOSMagicString[i],
					  pandaOSMagicString[i], pandaOSCorrectString[i]);
			while (true);
		}
	}

	log_info("Magic string is correct, entire kernel present!");
}
