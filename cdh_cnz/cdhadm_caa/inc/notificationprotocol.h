/*=================================================================== */
/**
	@file   notificationprotocol.h

	@brief
	This class is responsible for the protocol between CDH and the application.

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
#ifndef NOTIFICATIONPROTOCOL_H
#define NOTIFICATIONPROTOCOL_H

/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <fstream>
#include <utility>
#include <map>
#include <string>

using namespace std;

/*=====================================================================
                        TYPE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief ProtocolPair

          This class is responsible for the parameter value pair that is inserted in ProtocolMap.
 */
/*=================================================================== */
typedef pair< string , string  > ProtocolPair;
/*=================================================================== */
/**
   @brief ProtocolMap

         This class stores protocol parameters and value.
 */
/*=================================================================== */
typedef map< string , string , less<string>  > ProtocolMap;

/*=====================================================================
                CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     NotificationProtocol

                 This class is responsible for the protocol between CDH and the application.
 */
/*=================================================================== */
//
class NotificationProtocol 
{

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
		     	              CLASS DESTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**

		   	      @brief          ~NotificationProtocol
		   						  NotificationProtocol destructor.

		   		  @return          void

		   	      @exception       none
	 */
	/*=================================================================== */
	virtual ~NotificationProtocol();

	/*=====================================================================
    	     	              CLASS CONSTRUCTOR
    ==================================================================== */
	/*=================================================================== */
	/**

    	   	      @brief          NotificationProtocol
    	   						  NotificationProtocol constructor.

    	   		  @return         void

    	   	      @exception      none
	 */
	/*=================================================================== */
	NotificationProtocol ();
	/*===================================================================
    	 	                      ROUTINE DECLARATION SECTION
    =================================================================== */
	/*=================================================================== */
	/**

    					    	      @brief          add
    					    						  Adds a parameter value pair to the protocol.

    					    		  @param          parameter
    												  Name of the parameter
    					    	      @param          value
    												  Value for the parameter

    					    	      @return         true		on success
    												  false     on unsuccessful

    					    	      @exception      none
	 */
	/*=================================================================== */
	bool add (string parameter = "", string value = "");
	
	/*=================================================================== */
	/**

						    	      @brief          remove
						    						  Removes a parameter value pair from the protocol.

						    		  @param          parameter
    												  Name of the parameter

						    	      @return         true		on success
													  false     on unsuccessful

						    	      @exception      none
	 */
	/*=================================================================== */
	bool remove (string parameter = "");
	
	/*=================================================================== */
	/**

						    	      @brief          change
						    						  Changes the value for a parameter value pair.

						    		  @param          parameter
													  -
						    	      @param          value
													  -

						    	      @return         true		if value was changed
													  false     if value was unchanged

						    	      @exception      none
	 */
	/*=================================================================== */
	bool change (string parameter = "", string value = "");
	
	/*=================================================================== */
	/**

						    	      @brief          get
						    						  Retrieves the value for the parameter value pair.

						    		  @param          parameter
													  -

						    	      @return         string	-Returns the value, if not found the value is empty.

						    	      @exception      none
	 */
	/*=================================================================== */
	string get (string parameter = "");
	
	/*=================================================================== */
	/**

						    	      @brief          encode
						    						  Encodes the parameter value pairs to a string object supplied by user.

						    		  @param          encodedString
													  The string object will contain the parameters
													  and values separated with the newline character (\n).

						    	      @return         true		-if the parameter value pairs could be encoded
													  false     -if the parameter value pairs could not be encoded

						    	      @exception      none
	 */
	/*=================================================================== */
	bool encode (string& encodedString);
	
	/*=================================================================== */
	/**

						    	      @brief          decode
						    						  Decodes a string object and adds to parameter and value
													  pairs. Observe that previous values WILL be lost.

						    		  @param          codedString
													  -

						    	      @return         true		-if the codedString could be decoded
													  false     -if the codedString could not be decoded

						    	      @exception      none
	 */
	/*=================================================================== */
	bool decode (string codedString = "");
	
	/*=================================================================== */
	/**

						    	      @brief          compare
						    						  Compares one protocol to another.

						    		  @param          recvPrt
													  NotificationProtocol

						    	      @return         true		if identical protocols
													  false     if unidentical protocols

						    	      @exception      none
	 */
	/*=================================================================== */
	bool compare(NotificationProtocol recvPrt);
	
	/*=================================================================== */
	/**

						    	      @brief          ackReceived
						    						  Checks if the receiver has sent an ACK.

						    		  @param          parameter
													  -

						    	      @return         true		if ack is received
													  false     if ack is not received

						    	      @exception      none
	 */
	/*=================================================================== */
	bool ackReceived(string parameter = "");
	
	/*=================================================================== */
	/**

						    	      @brief          nakReceived
						    						  Checks if the receiver has sent an NAK.

						    		  @param          parameter
													  -

						    	      @return         true		if nak is received
													  false     if nak is not received

						    	      @exception      none
	 */
	/*=================================================================== */
	bool nakReceived(string parameter = "");

	/*=================================================================== */
	/**

						    	      @brief          clearPrt
						    						  Empties the parameter map.

						    	      @return         void

						    	      @exception      none
	 */
	/*=================================================================== */
	void clearPrt(void);

	/*=====================================================================
		                PRIVATE DECLARATION SECTION
	==================================================================== */
private:
	/*====================================================================
		                       PRIVATE ATTRIBUTES
	==================================================================== */

	/*=================================================================== */
	/**
				      @brief   parameterMap
							   -
	 */
	/*=================================================================== */
	ProtocolMap parameterMap;

};
#endif
