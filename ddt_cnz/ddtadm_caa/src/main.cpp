//	********************************************************
//
//	 COPYRIGHT Ericsson 2015
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2015.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2015 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	********************************************************

#include "daemon.h"
#include "common/programconstants.h"
#include "common/tracer.h"
#include "common/logger.h"

#include <getopt.h>

AES_DDT_TRACE_DEFINE(AES_DDT_Main)

namespace aes_ddt_cli
{
	inline void print_command_usage (const char * program_name)
	{
		fprintf(stderr, "USAGE: %s [--debug]\n", program_name);
	}

	const char * DEBUG_LONG_OPTION = "debug";

	static int debug_mode = 0;

	struct option long_options [] =
	{
			{DEBUG_LONG_OPTION, no_argument, &debug_mode, 1},
			{0, 0, 0, 0}
	};

	int parse (int _argc, char**_argv)
	{
		int debug_opt_count = 0;
		int long_index = 0;
		int opt_code = 0;
		const char * optstring = "d"; //contains short option characters

		while ((opt_code = ::getopt_long(_argc, _argv, optstring, long_options, &long_index)) != -1)
		{
			if (opt_code == 0) //A long option detected
			{
				if (::strcmp(long_options[long_index].name, DEBUG_LONG_OPTION) == 0)
				{
					//--debug option found
					if (debug_opt_count++)
					{
						//--_debug option provided more than one time
						::fprintf(::stderr, "%s: multiple instances of option '%s' \n", _argv[0], _argv[::optind - 1]);
						return -1;
					}
				}
				else
				{
					::fprintf(::stderr, "%s: Unrecognized option '%s'\n", _argv[0], _argv[::optind - 1]);
					return -1;
				}
			}
			else if (opt_code == 'd')
			{
				//--debug option found
				if (debug_opt_count++)
				{
					//--_debug option provided more than one time
					::fprintf(::stderr, "%s: multiple instances of option '%s' \n", _argv[0], _argv[::optind - 1]);
					return -1;
				}
				debug_mode = 1;

			}
			else if (opt_code == '?')
			{
				//The user provided an option not supported here
				::fprintf(::stderr, "%s: Unsupported option '%s'\n", _argv[0], _argv[::optind - 1]);
				return -1;
			}
			else if (opt_code == ':')
			{
				//The user missed the option argument. Here we have no such option, but if any in the future, please
				//start the optstring argument in getopt_long call with the ':' character.
				::fprintf(::stderr, "%s: Argument missed for the option '%s'\n", _argv[0], _argv[::optind - 1]);
				return -1;
			}
			else
			{
				//Other option found but not supported
				::fprintf(::stderr, "%s: Generic error parsing command line options\n", _argv[0]);
				return -1;
			}
		}
		return 0;
	}

}

int main (int argc, char**argv)
{
	// Parse command line
	if (aes_ddt_cli::parse(argc, argv) < 0)
	{
		aes_ddt_cli::print_command_usage(argv[0]);
		return common::PROGRAM_EXIT_BAD_INVOCATION;
	}

	// Start server work
	Daemon daemon;
	const int return_code = daemon.work(aes_ddt_cli::debug_mode);

	return return_code;
}


