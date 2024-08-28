/*=================================================================== */
/**
	@file   messageblock.h

	@brief
	This class handles messages block sent between threads.
	The source code is adapted for WinNT only.


	General rule:
	The error handling is specified for each method.
	No methods initiate or send error reports unless specified.

	@version 1.1.1
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
   N/A       27/09/2011     xbhadur       Initial Release
============================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef MESSAGE_BLOCK_H
#define MESSAGE_BLOCK_H

/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/Message_Block.h>
#include <ace/ACE.h>
#include <string>
#include <messagetype.h>

using namespace std;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     MessageBlock

                 This class handles messages block sent between threads.
 */
/*=================================================================== */
class MessageBlock : public ACE_Message_Block
{

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
			     	                        CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**

			   	      @brief          MessageBlock
			   						  MessageBlock constructor. A message type must follow the construction

			   		  @param          data
									  data.

			   		  @return         void

			   	      @exception      none
	 */
	/*=================================================================== */
	MessageBlock(const ACE_TCHAR *data = 0);

	/*=================================================================== */
	/**

			   	      @brief          MessageBlock
			   						  MessageBlock constructor.A message type must follow the construction

					  @param          messtype
									  messtype.

			   		  @return         void

			   	      @exception      none
	 */
	/*=================================================================== */
	MessageBlock(MessageType messtype);

	/*=====================================================================
			     	                        CLASS DESTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**

			   	      @brief          ~MessageBlock
			   						  MessageBlock destructor.

			   		  @return         void

			   	      @exception      none
	 */
	/*=================================================================== */
	virtual ~MessageBlock(void);

	/*===================================================================
		 	           ROUTINE DECLARATION SECTION
	 =================================================================== */
	/*=================================================================== */
	/**

						    	      @brief          msgType
						    						  Returns the message type for the message block


						    	      @return          MessageType		Returns the message type for the message block

						    	      @exception       none
	 */
	/*=================================================================== */
	MessageType msgType(void);
	
	/*=================================================================== */
	/**

							    	      @brief          msgType
							    						  Sets the message type for the message block.

							    		  @param          mtype
														  MessageType

							    	      @return          void

							    	      @exception       none
	 */
	/*=================================================================== */
	void msgType(MessageType mtype);
	
	/*=================================================================== */
	/**

							    	      @brief          setFileName
							    						  Sets the filename for the message block.

							    		  @param          fName
														  filename

							    	      @return          void

							    	      @exception       none
	 */
	/*=================================================================== */
	void setFileName(string &fName);

	/*=================================================================== */
	/**

							    	      @brief          getFileName
							    						  Returns the filename that the message block holds.

							    	      @return          string 	 filename. If the filename is not set, an empty string is returned.

							    	      @exception       none
	 */
	/*=================================================================== */
	string getFileName(void);

	/*=================================================================== */
	/**

							    	      @brief          setFileSize
							    						  Sets the filesize for the message block.

							    		  @param          fSize
														  filesize

							    	      @return          void

							    	      @exception       none
	 */
	/*=================================================================== */
	void setFileSize(string &fSize);

	/*=================================================================== */
	/**

							    	      @brief          getFileSize
							    						  Returns the filesize that the message block holds.

							    	      @return          string   -filesize. If the filesize is not set, an empty string is returned

							    	      @exception       none
	 */
	/*=================================================================== */
	string getFileSize(void);

	/*=================================================================== */
	/**

							    	      @brief          setMsgNr
							    						  Sets the filename for the message block.

							    		  @param          msgNr
														  message

							    	      @return          void

							    	      @exception       none
	 */
	/*=================================================================== */
	void setMsgNr(string &msgNr);

	/*=================================================================== */
	/**

							    	      @brief          getMsgNr
							    						  Returns the filename that the message block holds.

							    	      @return          string 	-filename. If the filename is not set, an empty string is returned.

							    	      @exception       none
	 */
	/*=================================================================== */
	string getMsgNr();
	
	/*=================================================================== */
        /**

                                                                      @brief          setResetFlag
                                                                                                          Sets the resetFlag for the message block.

                                                                      @param          rflag
                                                                                                                  bool

                                                                      @return          void

                                                                      @exception       none
         */
        /*=================================================================== */
        void setResetFlag(bool rflag);

	/*=================================================================== */
        /**

                                                                      @brief          getResetFlag
                                                                                                          Returns the resetFlag that the message block holds.

                                                                      @return          bool rflag. If the rflag is not set, false is returned.

                                                                      @exception       none
         */
        /*=================================================================== */
        bool getResetFlag();

	/*=====================================================================
	                        PRIVATE DECLARATION SECTION
	==================================================================== */
private:
	/*====================================================================
		                       PRIVATE ATTRIBUTES
	==================================================================== */

	/*=================================================================== */
	/**
			      @brief   mt
						   -
	 */
	/*=================================================================== */
	MessageType mt;
	/*=================================================================== */
	/**
				      @brief   fileName
							   -
	 */
	/*=================================================================== */
	string fileName;
	/*=================================================================== */
	/**
				      @brief   fileSize
							   -
	 */
	/*=================================================================== */
	string fileSize;
	/*=================================================================== */
	/**
				      @brief   msgNr
							   -
	 */
	/*=================================================================== */
	string msgNr;
	/*=================================================================== */
        /**
                                      @brief   resetFlag
                                                           -
         */
        /*=================================================================== */
        bool resetFlag;

};
#endif
