

#define PRINTF_DBO_ENABLED //change this value to avoid video printing

#if defined(PRINTF_DBO_ENABLED)
#	define PRINTF_LEVEL_DBG 1 // change level to print
#	define _PRINT_DBG_(x) ((printf("{%s:%d}",__PRETTY_FUNCTION__,__LINE__)) && (x))
#	define _PRINT_DBG_L(s,x) (((s)<=(PRINTF_LEVEL_DBG)) && (_PRINT_DBG_(x)))
#else
#	define _PRINT_DBG_L(s,x)
#endif


namespace aes_dbo_ns {
	typedef enum aesDboScreenPrintLevel { //add more level to add more details
	S_P_LEVEL_0 = 0,
	S_P_LEVEL_1,
	S_P_LEVEL_2,
	S_P_LEVEL_3,
	}sp_level_t;
}

#ifndef HEADER_GUARD_CLASS__aes_dbo_screen_print_level_handler
#define HEADER_GUARD_CLASS__aes_dbo_screen_print_level_handler aes_dbo_screen_print_level_handler

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__aes_dbo_screen_print_level_handler
class __CLASS_NAME__{
	inline __CLASS_NAME__(){}
	inline ~__CLASS_NAME__(){}

	int print_screen(aes_dbo_ns::sp_level_t /*level*/){ return 0;}

};
#endif
