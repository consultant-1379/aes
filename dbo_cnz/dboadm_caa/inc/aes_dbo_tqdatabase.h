//******************************************************************************
// 
// .NAME
//      TQDatabase - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME TQDatabase
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE <filename>

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//      <General description of the class>

// .ERROR HANDLING
//
//      General rule:
//      The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

// DOCUMENT NO
//      19089-CAA 109 0319

// AUTHOR 
//      1999-11-10 by UAB/I/LN  Urban Söderberg

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************


#ifndef AES_DBO_TQDATABASE_H
#define AES_DBO_TQDATABASE_H 

#include <aes_dbo_tqsession.h>

#include <list>
#include <string>

#define AES_DBO_RTR_CLIENT_CLASS   "AxeDataRecordBlockBasedJob"
#define AES_DBO_CPF_CLIENT_CLASS1  "AxeCpFileSystemInfiniteFile"

class destRec;

// The structure which the transfer queue list consists of
class FileRecord
{
public:
    char tq[256];               // DBO TQ name
    char destination[256];      // Destination set in CDH
    unsigned int mirror;        // 1 = mirror enabled, 0 = disabled
    unsigned int delay;         // Remove delay for the mirror files in minutes
    int open;                   // 1 = TQ is used, 0 = not used
    char for_future_use[995];
};


class aes_dbo_tqdatabase_impl
{
	friend class ACE_Singleton<aes_dbo_tqdatabase_impl, ACE_Recursive_Thread_Mutex>;

private:
	ACE_Recursive_Thread_Mutex mtx_;
    std::list<FileRecord *> list_;
    /**
     * @brief
     * constructor
     */
    aes_dbo_tqdatabase_impl();
    /**
     * @brief
     * destructor
     */
    ~aes_dbo_tqdatabase_impl();
    
public:
    /**
         *  defineTQ method: This method is used to define a TQ.
         *  @param  tq      :     const string
         *  @param  dest         : const string
         *  @param  mirror         : unsigned int
         *  @param  delay         : unsigned int
         */
    int defineTQ(const std::string &tq, const std::string &dest, unsigned int mirror, unsigned int delay);
    /**
         *  deleteTQ method: This method is used to delete a TQ.
         *  @param  tq      :     const string
         */
    int deleteTQ(const std::string &tq);
    /**
         *  changeTQDest method: This method is used to change the destination set associated with a TQ.
         *  @param  tq      :     const string
         *  @param  dest      :     const string   
         */
    int changeTQDest(const std::string &tq, const std::string &dest);
    /**
         *  changeTQMirror method: This method is used to change the TQ mirror.
         *  @param  tq      :     const string
         *  @param  mirror      :     unsigned int
         */ 
    int changeTQMirror(const std::string &tq, unsigned int mirror);
    /**
         *  changeTQDelay method: This method is used to change the TQ delay.
         *  @param  tq      :     const string
         *  @param  delay     unsigned int
         */ 
    int changeTQDelay(const std::string &tq, unsigned int delay);
    /**
         *  changeTQ method: This method is used to change the TQ.
         *  @param  tq      :     const string
         *  @param  dest         : const string
         *  @param  mirror         : unsigned int
         *  @param  delay         : unsigned int
         */
    int changeTQ(const std::string &tq, const std::string &dest, unsigned int mirror, unsigned int delay);
     /**
         *  tqList  method: This method is used to list TQs.
         *  @param  cmd      :     ACS_APGCC_Command
         *  @param  tqName   :  const string
         */
    int tqList(ACS_APGCC_Command &cmd, const std::string &tqName);
    /**
         *  isDefined  method: This method is used to check if a TQ is defined.
         *  @param  tq      :     const string
         */
    bool isDefined(const std::string &tq);
    /**
         *  isDefined  method: This method is used to check if a TQ is defined.
         *  @param  tq      :     const string
         *  @param  tqDN      :   string
         */
    bool isDefined(const std::string &tq, std::string &tqDN);
    /**
         *  isOpened  method: This method is used to check if a TQ is open.
         *  @param  tq      :     const string
         */
    bool isOpened(const std::string &tq);
    /**
         *  destinationUsed method :  This method is used to check the destination associated with a TQ.
         *  @param  tq      :     const string
         *  @param  dest      :     const string
         */    
    bool destinationUsed(const std::string &tq, const std::string &dest);
    /**
         *  open  method: This method is used to epen a TQ.
         *  @param  tq      :     const string
         */
    void open(const std::string &tq);
    /**
         *  close method: This method is used to close a TQ.
         *  @param  tq      :     const string
         */
    void close(const std::string &tq);
    /**
         *  isTQAttached method: This method is used toeck if a TQ is attached.
         *  @param  destset      :     string
         */
    bool isTQAttached(std::string &destSet);
    /**
         *  getDestinationSet method :  This method is used to get the destination set associated with a TQ.
         *  @param  tq      :     const string
         *  @param  dest      :     const string
         */
    unsigned int getDestinationSet(const std::string &tq, std::string &dest);
    /**
         *  getRemoveDelay method :  This method is used to get the remove delay  associated with a TQ.
         *  @param  tq      :     const string
         *  @param  delay      :  int  
         */
    int getRemoveDelay(const std::string &tq, int &delay);
    /**
         *  mirrored  method :  This method is used to check for mirror.
         *  @param  tq      :     const string
         */
    unsigned int mirrored(const std::string &tq);
    /**
         * getNrElements method :  This method is used to get the Nr elements.
         */
    int getNrElements(void);
    /**
         *  isTqDefined  method :  This method is used to check if a TQ is defined.
         *  @param  tq      :     const string
         */
    bool isTqDefined(const std::string &tq);
    /**
         *  cmpImmInfoInsTQ  method :  This method is used to insert TQ into a session.
         *  @param  tq      :     const string
         */
    int cmpImmInfoInsTQ(std::string tqName, bool, ACE_INT32, std::string);
    /**
         *  getTqDetails method :  This method is used to get TQ details .
         */
    bool getTqDetails(const std::string&, std::string&, unsigned int&, unsigned int&);
    /**
         *   clearTQListAtShutdown method :  This method is used to cleat the TQ list on shutdown.
         */
    void clearTQListAtShutdown();

};

typedef ACE_Singleton<aes_dbo_tqdatabase_impl, ACE_Recursive_Thread_Mutex> aes_dbo_tqdatabase;

#endif
