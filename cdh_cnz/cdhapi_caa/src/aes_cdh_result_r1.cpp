/*=================================================================== */
/**
   @file   aes_cdh_result_r1.cpp

   Class method implementation for AES_CDH_Result_R1 type module.

   This module contains the implementation of class declared in
   the aes_cdh_result_r1.h module

   Provides command results

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011   XCHEMAD     Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <string>
#include <iostream>
#include <aes_cdh_result.h>
#include <aes_cdh_resultimplementation.h>

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: AES_CDH_Result_R1
=================================================================== */
AES_CDH_Result_R1::AES_CDH_Result_R1():implementation(0) 
{
    implementation = new AES_CDH_ResultImplementation;
}

/*===================================================================
   ROUTINE: AES_CDH_Result_R1
=================================================================== */
AES_CDH_Result_R1::AES_CDH_Result_R1(const int code, 
                                     const std::string detail):implementation(0)
{
    implementation = new AES_CDH_ResultImplementation(code, detail);
}

/*===================================================================
   ROUTINE: ~AES_CDH_Result_R1
=================================================================== */
AES_CDH_Result_R1::~AES_CDH_Result_R1() 
{
	if( implementation != 0 )
	{
	    delete implementation;
	    implementation = 0;
	}
}

/*===================================================================
   ROUTINE: AES_CDH_Result_R1
=================================================================== */
AES_CDH_Result_R1::AES_CDH_Result_R1(const AES_CDH_Result_R1& res)
{
    if( implementation != 0 )
    {
	    delete implementation;
	    implementation = 0;
    }
    implementation = new AES_CDH_ResultImplementation(*(res.implementation));
}

/*===================================================================
   ROUTINE: =
=================================================================== */
AES_CDH_Result_R1& AES_CDH_Result_R1::operator=(const AES_CDH_Result_R1& res)
{
    if( implementation != 0 )
    {
	    delete implementation;
	    implementation = 0;
    }
    implementation = new AES_CDH_ResultImplementation(*(res.implementation));
    return *this;
}    

/*===================================================================
   ROUTINE: code
=================================================================== */
AES_CDH_ResultCode AES_CDH_Result_R1::code(void)
{
    return implementation->code();
}

/*===================================================================
   ROUTINE: errorText
=================================================================== */
string AES_CDH_Result_R1::errorText(void)
{
    return implementation->errorText();
}

/*===================================================================
   ROUTINE: detailInfo
=================================================================== */
string AES_CDH_Result_R1::detailInfo(void)
{
    return implementation->detailInfo();
}

/*===================================================================
   ROUTINE: <<
=================================================================== */
std::ostream& operator<<(std::ostream& s, const AES_CDH_Result_R1& res)
{
    s << *res.implementation;
    return s;
}

