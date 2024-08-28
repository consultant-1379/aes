/*=================================================================== */
/**
   @file aes_gcc_log.cpp

   Class method implementation for GCC TRA API

   This module contains the implementation of class declared in
   the acs_gcc_tra.h module

   @version 1.0.0
 */
/*

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       8/08/2011   XNADNAR   Initial Release
 */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <aes_gcc_log.h>
#include <ace/ACE.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <ACS_APGCC_Util.H>
using namespace std;

AES_GCC_Log *AES_GCC_Log::instance_ = 0;
ACS_TRA_Logging AES_GCC_Log::AES_GCC_Logging;

/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_Log::AES_GCC_Log()
{
	// Create the mutexes
	string myProcessName("");
	string logName("");
	ACS_APGCC::getProcessName(&myProcessName);
	if( myProcessName ==  AES_CDH_PROC_NAME )
	{
		logName = "CDH";
		AES_GCC_Logging.Open(logName.c_str());
	}
	else if (  myProcessName == AES_AFP_PROC_NAME )
	{
		logName = "AFP";
		AES_GCC_Logging.Open(logName.c_str());
	}
	else if ( myProcessName == AES_DBO_PROC_NAME )
	{
		logName = "DBO";
		AES_GCC_Logging.Open(logName.c_str());
	}
		
}

/*===================================================================
                    ROUTINE: getString
=================================================================== */
std::string AES_GCC_Log::getString(const char *format, va_list argp)
{
	stringstream out;

	// Walk through the format string and create a output string
	while (*format != 0)
	{
		// If it is normal text, just add it
		if (*format != '%')
			out << *format++;

		// Check for a escaped percent-sign
		else if (format[1] == '%')
		{
			out << *format++;
			format++;
		}

		// It is some kind of control character to expand
		else
		{
			format++;
			switch (*format++)
			{
			case 'B':       // Bool as strings true or false
				if(va_arg(argp, int)==1)
				{
					out << "true";
				}
				else
				{
					out<<"false";
				}
				break;

			case 'b':       // Bool as strings true or false
				if(va_arg(argp, int)==1)
				{
					out << "true";
				}
				else
				{
					out<<"false";
				}
				break;

			case 'd':       // Decimal
				out << va_arg(argp, int);
				break;

			case 'p':       // Process ID (PID)
				out << ACE_OS::getpid();
				break;

			case 'S':       // STL string
				out << va_arg(argp, char*);
				break;

			case 's':       // char * string
				out << va_arg(argp, char *);
				break;

			case 't':       // Thread ID (TID)

				out << pthread_self();
				break;

			case 'u':       // Unsigned decimal
				out << va_arg(argp, unsigned int);
				break;

			default:
				// ?????
				break;
			}
		}
	}
	return out.str();
}

/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
AES_GCC_Log::~AES_GCC_Log()
{
	logMutex_.remove();
	traceMutex_.remove();
	//AES_GCC_Logging.Close();
}

/*===================================================================
                    ROUTINE: log
=================================================================== */


void AES_GCC_Log::log(const char *format, ...)
{
        // Make sure that we are alone
        logMutex_.acquire();
        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        //    logTarget_ << out;
        //    logTarget_.flush();
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_DEBUG);
        va_end(argp);
        // Let others in
        logMutex_.release();
}
void AES_GCC_Log::logDebug(const char *format, ...)
{
	// Make sure that we are alone
	logMutex_.acquire();
	va_list argp;
	va_start(argp, format);
	string out = getString(format, argp);
	//    logTarget_ << out;
	//    logTarget_.flush();
	AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_DEBUG);
	va_end(argp);
	// Let others in
	logMutex_.release();
}
void AES_GCC_Log::logTrace(const char *format, ...)
{
        // Make sure that we are alone
        logMutex_.acquire();
        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        //    logTarget_ << out;
        //    logTarget_.flush();
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_TRACE);
        va_end(argp);
        // Let others in
        logMutex_.release();
}
void AES_GCC_Log::logInfo(const char *format, ...)
{
        // Make sure that we are alone
        logMutex_.acquire();
        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        //    logTarget_ << out;
        //    logTarget_.flush();
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_INFO);
        va_end(argp);
        // Let others in
        logMutex_.release();
}
void AES_GCC_Log::logWarn(const char *format, ...)
{
        // Make sure that we are alone
        logMutex_.acquire();
        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        //    logTarget_ << out;
        //    logTarget_.flush();
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_WARN);
        va_end(argp);
        // Let others in
        logMutex_.release();
}
void AES_GCC_Log::logError(const char *format, ...)
{
        // Make sure that we are alone
        logMutex_.acquire();
        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        //    logTarget_ << out;
        //    logTarget_.flush();
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_ERROR);
        va_end(argp);
        // Let others in
        logMutex_.release();
}
void AES_GCC_Log::logFatal(const char *format, ...)
{
        // Make sure that we are alone
        logMutex_.acquire();
        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        //    logTarget_ << out;
        //    logTarget_.flush();
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_FATAL);
        va_end(argp);
        // Let others in
        logMutex_.release();
}


/*===================================================================
                    ROUTINE: log
=================================================================== */
void AES_GCC_Log::log(ACS_TRA_trace& , const char *format, ...)
{

        // Make sure that we are alone
        logMutex_.acquire();

        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_DEBUG);
        va_end(argp);
        // Let others in
        logMutex_.release();

}


void AES_GCC_Log::logDebug(ACS_TRA_trace& , const char *format, ...)
{

	// Make sure that we are alone
	logMutex_.acquire();

	va_list argp;
	va_start(argp, format);
	string out = getString(format, argp);
	AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_DEBUG);
	va_end(argp);
	// Let others in
	logMutex_.release();

}

void AES_GCC_Log::logTrace(ACS_TRA_trace& , const char *format, ...)
{

        // Make sure that we are alone
        logMutex_.acquire();

        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_TRACE);
        va_end(argp);
        // Let others in
        logMutex_.release();

}
void AES_GCC_Log::logInfo(ACS_TRA_trace& , const char *format, ...)
{

        // Make sure that we are alone
        logMutex_.acquire();

        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_INFO);
        va_end(argp);
        // Let others in
        logMutex_.release();

}
void AES_GCC_Log::logWarn(ACS_TRA_trace& , const char *format, ...)
{

        // Make sure that we are alone
        logMutex_.acquire();

        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_WARN);
        va_end(argp);
        // Let others in
        logMutex_.release();

}
void AES_GCC_Log::logError(ACS_TRA_trace& , const char *format, ...)
{

        // Make sure that we are alone
        logMutex_.acquire();

        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_ERROR);
        va_end(argp);
        // Let others in
        logMutex_.release();

}
void AES_GCC_Log::logFatal(ACS_TRA_trace& , const char *format, ...)
{

        // Make sure that we are alone
        logMutex_.acquire();

        va_list argp;
        va_start(argp, format);
        string out = getString(format, argp);
        AES_GCC_Logging.Write(out.c_str(),LOG_LEVEL_FATAL);
        va_end(argp);
        // Let others in
        logMutex_.release();

}


/*===================================================================
                    ROUTINE: trace
=================================================================== */
void AES_GCC_Log::trace(ACS_TRA_trace &tp, const char *format, ...)
{
	if (tp.ACS_TRA_ON())
	{
		// Make sure that we are alone
		traceMutex_.acquire();
		va_list argp;
		va_start(argp, format);
		string out = getString(format, argp);
		string::iterator it = out.end();
		it--;
		if (*it == '\n')
			out.erase(it);
		tp.ACS_TRA_event(1, out.c_str());

		va_end(argp);

		// Let others in
		traceMutex_.release();
	}
}

/*===================================================================
                    ROUTINE: instance
=================================================================== */
AES_GCC_Log *AES_GCC_Log::instance()
{
	if (!instance_)
	{
		static AES_GCC_Log theGccLogObj;
		return instance_ = &theGccLogObj;
	}
	else
	{
		return instance_;
	}
}




