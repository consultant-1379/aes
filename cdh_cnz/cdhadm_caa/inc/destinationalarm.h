/*=================================================================== */
/**
	@file   destinationalarm.h

	@brief
	DestinationAlarm class serves as the API to the alarm handling functionality.It uses a nested class (AlarmRecord). There shall be one alarm
	record for each destination. This alarm record stores all
	subscribers (if any) for that destination.

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
#ifndef DESTINATIONALARM_H
#define DESTINATIONALARM_H

/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <cmdhandler.h>
#include <ace/Mutex.h>
#include <servr.h>
#include <transdestset.h>
#include <aes_cdh_destinationset.h>
#include <ace/ACE.h>
#include <map>
#include <string>
#include <list>

using namespace std;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     SubscriberStruct

                 subscribers is used for alarm record storage for the destination
*/
/*=================================================================== */
class SubscriberStruct
{

	/*=====================================================================
	                        PRIVATE DECLARATION SECTION
	==================================================================== */
private:
	/*====================================================================
		                        PRIVATE ATTRIBUTES
	==================================================================== */
	/*=================================================================== */
	/**
				  @brief   cmdH_

						   command
   */
   /*=================================================================== */
    CmdHandler *cmdH_;
    /*=================================================================== */
    	/**
    				  @brief   alarmText_

							   Text to be alarmed
       */
    /*=================================================================== */
    string alarmText_;
    /*=================================================================== */
    	/**
    				  @brief   fileDestination_

							   file destination
       */
    /*=================================================================== */
    string fileDestination_;

    /*=================================================================== */
    	/**
    				  @brief   bConnectErrSent_

							   This flag is true only if CONNECTERR has been reported thru eventDestination for this particular subscriber
       */
   /*=================================================================== */
    bool bConnectErrSent_;

    /*=====================================================================
                            PUBLIC DECLARATION SECTION
    ==================================================================== */
public:
    /*===================================================================
                               ROUTINE DECLARATION SECTION
    =================================================================== */
    /*=====================================================================
       	                        CLASS CONSTRUCTOR
    ==================================================================== */
   	/*=================================================================== */
   	/**

   	      @brief          SubscriberStruct
   						  SubscriberStruct constructor.

   		  @return          void

   	      @exception       none
   	 */
   	/*=================================================================== */
    SubscriberStruct(CmdHandler *cmdHdlr);

    /*=================================================================== */
       	/**

       	      @brief          SubscriberStruct
       						  SubscriberStruct constructor.

       		  @return          void

       	      @exception       none
       	 */
  /*=================================================================== */
    SubscriberStruct(CmdHandler *cmdHdlr, const string &fileDestination, const string &alarmText);
    /*=====================================================================
       	                        CLASS DESTRUCTOR
      	==================================================================== */
   	/*=================================================================== */
   	/**

   	      @brief          SubscriberStruct
   						  SubscriberStruct destructor.

   		  @return          void

   	      @exception       none
   	 */
   	/*=================================================================== */
    virtual ~SubscriberStruct();
    /*=================================================================== */
    	/**

    	      @brief          getAlarmText
    						  To get the alarm text.

    		  @return          String    The text of ALarm

    	      @exception       none
    	 */
    /*=================================================================== */
    string getAlarmText();
    /*=================================================================== */
    	/**

    	      @brief          getFileDestination
    						  To get the file destination.

    	      @return          String    returns fileDestination_.

    	      @exception       none
    	 */
    /*=================================================================== */
    string getFileDestination();

    /*=================================================================== */
    	/**

    	      @brief          getCmdHandler
    						  to get pointer to cammad handler.

    	      @return          CmdHandler    returns cmdH_.

    	      @exception       none
    	 */
   /*=================================================================== */
    CmdHandler *getCmdHandler();

    /*=================================================================== */
    	/**

    	      @brief          getConnectErrSent
    						  To get the status of error sent.

    	      @return          true    to indicate if bConnectErrSent_ is true.
    						   false   to indicate if bConnectErrSent_ is false.

    	      @exception       none
    	 */
   /*=================================================================== */
    bool getConnectErrSent() const;

    /*=================================================================== */
    	/**

    	      @brief          setConnectErrSent
    						  To set bConnectErrSent_

    		  @param           err	
      							boolean used to set bConnectErrSent_

    	      @return          none

    	      @exception       none
    	 */
   /*=================================================================== */
    void setConnectErrSent(const bool err);
};


/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     DestinationAlarm

                 This class serves as the API to the alarm handling functionality.
				 It uses a nested class (AlarmRecord). There shall be one alarm
				 record for each destination. This alarm record stores all
				 subscribers (if any) for that destination.
*/
/*=================================================================== */

void getAP(string &);
void getHostname(std::string &);
void getNodeLetter(string &);
class DestinationAlarm
{
 	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	static string myDestSetNm_;
	/*=====================================================================
	                        CLASS DECLARATION SECTION
	==================================================================== */
	/*===================================================================*/
	/**
	      @brief     AlarmRecord

	                 There shall be one alarm record for each destination.
	*/
	/*=================================================================== */
    class AlarmRecord
    {
    	/*=====================================================================
    	                        PUBLIC DECLARATION SECTION
    	==================================================================== */
    public:
    	/*====================================================================
    			                        PRIVATE ATTRIBUTES
    	==================================================================== */
    	/*=====================================================================
    	                        ENUMERATED DECLARATION SECTION
    	==================================================================== */
    	/*=================================================================== */
    	/**
    	   @brief  thisCause

    	           Detailed description
    	*/
    	/*=================================================================== */
		enum thisCause 
		{
			ok,
			connect_error,
			connect_error_fn,
			write_error,
			destination_set_error,
			vd_error,	// added for TR HH91483
			SU_error  //Added for Ready Directory Monitoring Functionality IP
		};

		/*=====================================================================
		    	                        CLASS CONSTRUCTOR
		==================================================================== */
		/*=================================================================== */
			/**

			      @brief          AlarmRecord
								  AlarmRecord constructor.

				  @param          dest
								  destination

				  @return          void

			      @exception       none
			 */
		/*=================================================================== */
        AlarmRecord(const string &dest);

        /*=====================================================================
           	                        CLASS DESTRUCTOR
        ==================================================================== */
       	/*=================================================================== */
       	/**

       	      @brief          AlarmRecord
       						  AlarmRecord destructor.

       		  @return          void

       	      @exception       none
       	 */
      /*=================================================================== */
//		AlarmRecord(ReadyPoll *PollVdAlarmObj);	
        virtual ~AlarmRecord() {}

        /*=================================================================== */
            	/**

            	      @brief          insertSubscriber
            						  Inserts a subscriber into the subscriber list.

            		  @param           	sub
              							pointer to SubscriberStruct
              		  @param           	destSet
              							destination set

            	      @return          void

            	      @exception       none
            	 */
         /*=================================================================== */
        void insertSubscriber(SubscriberStruct *sub, const std::string &destSet);

        /*=================================================================== */
            	/**

            	      @brief          removeSubscriber
            						  Removes a subscriber from the subscriber list

            		  @param           sub
              						   pointer to SubscriberStruct

            	      @return          void

            	      @exception       none
            	 */
        /*=================================================================== */
        void removeSubscriber(SubscriberStruct *sub);

        /*=================================================================== */
            	/**

            	      @brief          destroy
            						  Destroys the entire alarm record

            	      @return          void

            	      @exception       none
            	 */
        /*=================================================================== */
        void destroy();

        /*=================================================================== */
            	/**

            	      @brief          raise
            						  -

            		  @param           	destSet
              							destination set
              		  @param           	alarmCause
              							alarmCause
            		  @param           	GlbDestPath
              							GlbDestPath
              		  @param           	CDH_Ready_File_Number
              							CDH_Ready_File_Number

            	      @return          void

            	      @exception       none
            	 */
        /*=================================================================== */
		void raise(const std::string& destSet, thisCause   alarmCause,
						std::string GlbDestPath = " ", ACE_UINT64 CDH_Ready_File_Number = 0, ACE_INT64 OldestFileTimeStamp = 0);

		/*=================================================================== */
		    	/**

		    	      @brief          cease
		    						  -

		    		  @param           	destSet
		      							destination set
		      		  @param           	alarmCause
              							alarmCause
            		  @param           	GlbDestPath
              							GlbDestPath
              		  @param           	CDH_Ready_File_Number
              							CDH_Ready_File_Number

		    	      @return          true    to indicate that everything is OK.
		    						   false   to indicate that something went wrong.

		    	      @exception       none
		    	 */
		 /*=================================================================== */
		bool cease(const std::string& destSet, thisCause   alarmCause,
						std::string GlbDestPath = " ", ACE_UINT64 CDH_Ready_File_Number = 0, ACE_INT64 OldestFileTimeStamp = 0);

		/*=================================================================== */
		    	/**

		    	      @brief          createProblemText
		    						  -

		    		  @param           alarmCause
              						   alarmCause
            		  @param           GlbDestPath
              						   GlbDestPath
              		  @param           CDH_Ready_File_Number
              						   CDH_Ready_File_Number

		    	      @return          void

		    	      @exception       none
		    	 */
		/*=================================================================== */
		bool createProblemText( thisCause   alarmCause,
						std::string GlbDestPath = " ", ACE_UINT64 CDH_Ready_File_Number = 0, ACE_INT64 OldestFileTimeStamp = 0);

		
		/*=====================================================================
		                        PRIVATE DECLARATION SECTION
		==================================================================== */
    private:      
		/*====================================================================
				                        PRIVATE ATTRIBUTES
		==================================================================== */
		/*=====================================================================
		                        ENUMERATED DECLARATION SECTION
		==================================================================== */
		/*=================================================================== */
		/**
		   @brief  previousCause

		           -
		*/
		/*=================================================================== */
		enum previousCause 
		{
			ok_prev,
			connect_error_prev,
			write_error_prev,
			vd_error_prev,	// added for TR HH91483
			SU_error_prev  //Added for Ready Directory Monitoring Functionality IP
		};
		/*=================================================================== */
				/**
								  @brief   cause1_

										   cause
				*/
		/*=================================================================== */
      static const string cause1_;

      /*=================================================================== */
      		/**
      						  @brief   cause2_

      								   cause
      		*/
     /*=================================================================== */
      static const string cause2_;

      /*=================================================================== */
      		/**
      						  @brief   eventCode_

      								   eventCode
      		*/
      /*=================================================================== */
      static const ACE_INT32 eventCode_;

      /*=================================================================== */
      		/**
      						  @brief   eventCode1_

      								   For CDH Ready Directory Polling Functionality.
      		*/
      /*=================================================================== */
		static const ACE_INT32 eventCode1_;

		/*=================================================================== */
				/**
								@brief   destAlarmText_

										 destination Alram text
				*/
		/*=================================================================== */
		static const string destAlarmText_;
		/*=================================================================== */
				/**
								  @brief   destSetAlarmText_

										   alram text to be set
				*/
		/*=================================================================== */
		static const string destSetAlarmText_;

		/*=================================================================== */
				/**
								  @brief   cause_1

										   cause
				*/
		/*=================================================================== */
		static const string cause_1;

		/*=================================================================== */
				/**
								  @brief   problemText1_

										   problemText
				*/
		/*=================================================================== */
		static const string problemText1_;

		/*=================================================================== */
				/**
								  @brief   problemText2_

										   problemText
				*/
		/*=================================================================== */
		static const string problemText2_;

		/*=================================================================== */
				/**
								  @brief   Vcause_

										   cause
				*/
		/*=================================================================== */
		static const string Vcause_;

		/*=================================================================== */
				/**
								  @brief   VAlarmText_

										   AlarmText
				*/
		/*=================================================================== */
		static const string VAlarmText_;

		/*=================================================================== */
				/**
								  @brief   problemText3_

										   problemText
				*/
		/*=================================================================== */
		static const string problemText3_;
		
		/*=================================================================== */
				/**
								  @brief   SUcause_

										   cause
				*/
		/*=================================================================== */
		static const string SUcause_;

		/*=================================================================== */
				/**
								  @brief   SUAlarmText_

										   Alarm text
				*/
		/*=================================================================== */
		static const string SUAlarmText_;

		/*=================================================================== */
				/**
								  @brief   SUproblemText4_

										   problem text
				*/
		/*=================================================================== */
		static const string SUproblemText4_;

	  /*=================================================================== */
				/**
								  @brief   dest_

										   destination
				*/
	  /*=================================================================== */
      string dest_;

      /*=================================================================== */
      		/**
      						  @brief   bAlarmRaised_

      								   If true, there is an alarm raised for this destination
      		*/
      /*=================================================================== */
      bool bAlarmRaised_;

      /*=================================================================== */
      		/**
      						  @brief   problemText_

      								   problemText
      		*/
      /*=================================================================== */
  	  string problemText_;

  	/*=================================================================== */
  			/**
  							  @brief   oldCause

  									   oldCause
  			*/
   /*=================================================================== */
  	  previousCause oldCause;

  	/*=================================================================== */
  			/**
  							  @brief   cause_

  									   cause
  			*/
  	/*=================================================================== */
      string cause_;

    /*=================================================================== */
      		/**
      						  @brief   defaultAlarmText_

      								   default Alarm text
      		*/
    /*=================================================================== */
      string defaultAlarmText_;

    /*=================================================================== */
      		/**
      						  @brief   subscriberList_

      								   list of SubscriberStruct
      		*/
    /*=================================================================== */
      std::list<SubscriberStruct *> subscriberList_;


	};
    /*=====================================================================
                            PUBLIC DECLARATION SECTION
    ==================================================================== */
public:
    /*===================================================================
                               PUBLIC METHODS
    =================================================================== */
    /*=================================================================== */
        	/**

        	      @brief          raise
        						  To send the event is sent to subscriber.

        		  @param           	dest
          							destination
          		  @param           	destSet
          							destination set
        		  @param           	alarmCause
          							alarmCause
          		  @param           	GlbDestPath
          							GlbDestPath
				  @param           	CDH_Ready_File_Number
          							CDH_Ready_File_Number
          		  @param           	OldestFileTimeStamp
          							OldestFileTimeStamp

        	      @return          void

        	      @exception       none
        	 */
    /*=================================================================== */
	//destSet == "" => no event is sent to subscriber (used 
	//by notification)
	void raise(const std::string& dest, const std::string& destSet, AlarmRecord::thisCause   alarmCause,
					std::string GlbDestPath = " ", ACE_UINT64 CDH_Ready_File_Number = 0, ACE_INT64 OldestFileTimeStamp = 0);

	/*=================================================================== */
	    	/**

	    	      @brief          cease
	    						  -

	    		  @param           	dest
	      							destination
	      		  @param           	destSet
          							destination set
        		  @param           	alarmCause
          							alarmCause
          		  @param           	GlbDestPath
          							GlbDestPath
				  @param           	CDH_Ready_File_Number
          							CDH_Ready_File_Number
          		  @param           	OldestFileTimeStamp
          							OldestFileTimeStamp

	    	      @return          void

	    	      @exception       none
	    	 */
	/*=================================================================== */
	void cease(const std::string& dest, const std::string& destSet, AlarmRecord::thisCause   alarmCause,
					std::string GlbDestPath = " ", ACE_UINT64 CDH_Ready_File_Number = 0, ACE_INT64 OldestFileTimeStamp = 0);

	/*=================================================================== */
	    	/**

	    	      @brief          removeDestSet
	    						  removes destSet from destPair_ map

	    		  @param          destSet
	      						  destination set

	    	      @return          void

	    	      @exception       none
	    	 */
	/*=================================================================== */
	void removeDestSet(const std::string& destSet);


	/*=================================================================== */
	    	/**

	    	      @brief          instance
	    						  Returns a pointer to the one and only instance of this object.
								  The pointer shall be used to execute the methods below

	    	      @return         DestinationAlarm    Returns a pointer to the one and only instance of this object

	    	      @exception       none
	    	 */
	 /*=================================================================== */
    static DestinationAlarm *instance();


    /*=================================================================== */
        	/**

        	      @brief          insertSubscriber
        						  Inserts a subscriber into the alarm record for the specified destination

        		  @param          dest
          						  destination
          		  @param          sub
          						  sub
        		  @param          destSet
          						  destination set

        	      @return          void

        	      @exception       none
        	 */
    /*=================================================================== */
    void insertSubscriber(const std::string &dest, SubscriberStruct *sub, const std::string &destSet);

    /*=================================================================== */
        	/**

        	      @brief          removeSubscriber
        						   Removes a subscriber from the alarm for the specified destination

        		  @param           	dest
          							destination
          		  @param           	sub
          							pointer to SubscriberStruct

        	      @return          void

        	      @exception       none
        	 */
       /*=================================================================== */
    void removeSubscriber(const std::string &dest, SubscriberStruct *sub);

    /*=================================================================== */
        	/**

        	      @brief          destroyDestinationEntry
        						  Destroys the entire entry in the database for the specified destination.

        		  @param          dest
          						  destination

        	      @return          void

        	      @exception       none
        	 */
    /*=================================================================== */
    void destroyDestinationEntry(const std::string &dest);
    // Destroys the entire entry in the database for the
    // specified destination.

    /*=================================================================== */
        	/**

        	      @brief          clearMapsAtShutDown
        						  Clears the destPair_ and destMap_ at shutdown.

        	      @return          void

        	      @exception       none
        	 */
    /*=================================================================== */
	void clearMapsAtShutDown();
    /*=================================================================== */

    /*=====================================================================
                            PRIVATE DECLARATION SECTION
    ==================================================================== */
	
private:

    /*=====================================================================
       	                        CLASS CONSTRUCTOR
    ==================================================================== */
   	/*=================================================================== */
   	/**

   	      @brief          DestinationAlarm
   						  DestinationAlarm constructor.

   		  @return          void

   	      @exception       none
   	 */
   	/*=================================================================== */
    DestinationAlarm();
    /*=====================================================================
       	                        CLASS DESTRUCTOR
     ==================================================================== */
   	/*=================================================================== */
   	/**

   	      @brief          ~DestinationAlarm
   						  DestinationAlarm destructor.

   		  @return          void

   	      @exception       none
   	 */
   	/*=================================================================== */
    virtual ~DestinationAlarm();
    
    /*=================================================================== */
    		/**
    						  @brief   instance_

    								   Pointer to the only instance of this object
    		*/
   /*=================================================================== */
    static DestinationAlarm *instance_;

    /*=================================================================== */
    		/**
    						  @brief   destMap_

    								   Map that represents the main database, using destination name as key.
									   Thus, there may be one alarm record per destination
    		*/
    /*=================================================================== */
    std::map<std::string, AlarmRecord *> destMap_;

    /*=================================================================== */
    		/**
    						  @brief   mutex_

    								   mutex
    		*/
   /*=================================================================== */
//    ACE_Mutex mutex_;
ACE_Recursive_Thread_Mutex mutex_;


    /*=================================================================== */
    /**
        @struct  TransDestData

        @brief   Type brief Description

                 Destination Set Alarm start

        @par priOk
             -
        @par secOk
             -
        @par backOk
             -
        @par priDestName
             -
        @par secDestName
             -
        @par backDestName
             -
        @par tds
             -
    */
    /*=================================================================== */
	//
	struct TransDestData {
       bool priOk;
	   bool secOk;
	   bool backOk;
	   string priDestName;
	   string secDestName;
	   string backDestName;
	   class TransDestSet *tds;
   };

	/*=================================================================== */
			/**
							  @brief   destpairMX_

									   destination pair mutex
			*/
	/*=================================================================== */
	//ACE_Mutex destpairMX_;
	ACE_Recursive_Thread_Mutex destpairMX_;
	/*=================================================================== */
			/**
							  @brief  destAlarmInstMx_
								DestinationAlarm::instance mutex
			*/
	/*=================================================================== */
	static ACE_Mutex destAlarmInstMx_;

	/*=================================================================== */
                        /**
                                   @brief  subscriberListMX_
            		                   Mutex for subscriberList_
                        */
        /*=================================================================== */

	static ACE_Recursive_Thread_Mutex subscriberListMX_;

	/*=================================================================== */
			/**
				  @brief   destPair_
					   destination pair
			*/
   /*=================================================================== */
   map<string, TransDestData> destPair_;

   /*=====================================================================
                           TYPE DECLARATION SECTION
   ==================================================================== */
   /*=================================================================== */
   /**
			  @brief DestSet_Pair
					 pair of destinationsets.
    */
   /*=================================================================== */
   typedef pair<string, TransDestData> DestSet_Pair;

};

#endif
