/*=================================================================== */
/**
   @file   aes_cdh_destination_r1.cpp

   Class method implementationn for AES_CDH_Destination_R1 type module.

   This module contains the implementation of class declared in
   the aes_cdh_destination_r1.h module

   This program receives parameters sent from programs
   cdhdef and cdhls. The parameters is then analysed and
   sent further to method sendCmd in class Destination.
   Cdhdef is calling method define and cdhls is calling
   method getAttr. The different parts is described below.

   cdhdef/define : The arguments are supplyed in the argument
   list when the program is activated. (example prompt> cdhdef
   -d dest1 -a address -r c:main -t iforpc). The arguments -d
   (destination name) and  and -a (host address) are required.
   The arguments -a and -t (transfer type (IFORPC for example))
   are checked and stored in the variables "destination" and
   "transtype". Default value for -t is IFORPC. The arguments
   -a and -t are removed from the argument list. The variables
   destination, transtype and the remaining arguments are sent
   further to method sendCmd in class destination.

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
#include <aes_cdh_destination.h>
#include <aes_cdh_destinationimplementation.h>

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: AES_CDH_Destination_R1
=================================================================== */
AES_CDH_Destination_R1::AES_CDH_Destination_R1() 
{
    implementation = new AES_CDH_DestinationImplementation;
}

/*===================================================================
   ROUTINE: AES_CDH_Destination_R1
=================================================================== */
AES_CDH_Destination_R1::AES_CDH_Destination_R1(const std::string& destName) 
{
    implementation = new AES_CDH_DestinationImplementation(destName);
}

/*===================================================================
   ROUTINE: ~AES_CDH_Destination_R1
=================================================================== */
AES_CDH_Destination_R1::~AES_CDH_Destination_R1() 
{
    delete implementation;
}

/*===================================================================
   ROUTINE: exists
=================================================================== */
AES_CDH_ResultCode AES_CDH_Destination_R1::exists(void)
{
    return implementation->exists();
}

/*===================================================================
   ROUTINE: getError
=================================================================== */
AES_CDH_ResultCode AES_CDH_Destination_R1::getError(void)
{
    return implementation->getError();
}

/*===================================================================
   ROUTINE: getErrorText
=================================================================== */
void AES_CDH_Destination_R1::getErrorText(std::string& errText)
{
    implementation->getErrorText(errText);
}

/*===================================================================
   ROUTINE: getAttr
=================================================================== */
AES_CDH_Result AES_CDH_Destination_R1::getAttr(int argc, char* argv[],
                    std::vector<AES_CDH_Destination::destAttributes>& attr)
{
    return AES_CDH_DestinationImplementation::getAttr(argc, argv, attr);
}
