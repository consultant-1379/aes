/**********************************************************************
 * File:		Supervisor.h                                          *
 * Date:		2000-07-06                                            *
 * Description: Class that handle vaules used in GHOST.               *        
 **********************************************************************/
#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <string>
#include <vector>
#include <ace/ACE.h>

// FLAGS USED DURING DEVELOPMENT
#define DEBUG 0   // Set to 1 to enable trace
#define RESCUE 1

#define NR_OF_THRDS		100  // Change to dynamic!?
                             //  0    1 2  3 4     5 6  7			  8
#define NR_OF_INT_OPT	9	 // |Dest|a|f |l|s    |z|t |lastSentFile |b
#define NR_OF_STR_OPT	8	 // |Dest|p|pl|m|State|e|i |streamId	

class Supervisor {

	private:
		ACE_INT32 aDefValue_;              // Amount (Nr of files to put in each directory)
		ACE_INT32 bDefValue_;              // Amount of threads in multicp send block
		ACE_INT32 dDefValue_;				// Destinations
		ACE_INT32 fDefValue_;				// Frequency
		ACE_INT32 lDefValue_;				// Record length
		ACE_INT32 sDefValue_;				// Nr of records
		ACE_INT32 zDefValue_;				// Transaction length for block tranfer
		ACE_INT32 tDefValue_;				// Transfer type (ascii/bin (0/1))
		ACE_INT32 lastSentFileDefValue_;	// Last correct file sent to GOH
	
		std::string pathDefValue_;		// Path
		std::string logPathDefValue_;	// Log path
		std::string maskDefValue_;		// Mask
		std::string eventTextDefValue_;	// Event text
		std::string thrStateDefValue_;	// Thread status (Running, Paused, Stopped)
		std::string rpcType_;			// RpcType (BGW,IFO)
		std::string streamIDDefValue_;	// streamID
		std::string defFolder_;			// Default folder for gohst files and log file
		ACE_INT32 intOptions_[NR_OF_THRDS][NR_OF_INT_OPT]; 
		std::string defDisc_;

		std::string strOptions_[NR_OF_THRDS][NR_OF_STR_OPT];
		std::string rescueDirectory_;
		std::string rescueFileName_;
	
		bool debugEnabled_;
//		bool checkEnabled_;

	protected:

	public:
		// Constructor
		// Modified from qmicsal 21/02/2007
		Supervisor(ACE_TCHAR * pCPName);
		// End Modified from qmicsal

		// Destructor
		~Supervisor();

		/*** Setters ***/
		
		void defaultIntOptions();

		void defaultStrOptions();

		void defaultValues();

		void setIntOptions(ACE_INT32 thrId, ACE_INT32 opt, ACE_INT32 value);

		void setStrOptions(ACE_INT32 thrId, ACE_INT32 opt, std::string value);

		void setNumberOfDest(ACE_INT32 Dest){dDefValue_ = Dest;}

		void setDebugMode(bool mode=false){debugEnabled_ = mode;}

	//	void setCheckMode(bool mode=false){checkEnabled_ = mode;}
		
		void setDefaultDisc(std::string disc){defDisc_ = disc;}
	
		/*** Getters ***/
		
		ACE_INT32 getIntOptions(ACE_INT32 thrId, ACE_INT32 opt);
		
		std::string getStrOptions(ACE_INT32 thrId, ACE_INT32 opt);
		
		ACE_INT32 getNumberOfDest(){return dDefValue_;}
		
		bool getDebugMode(){return debugEnabled_;}
		
	//	bool getCheckMode(){return checkEnabled_;}

		std::string getDefaultPath(){return pathDefValue_;}

		std::string getDefaultLogPath(){return logPathDefValue_;}

		std::string getDefaultDisc(){return defDisc_;}


		/*** Misc functions ***/
		
		void print();
		
		void printex();
		
		ACE_INT32 checkIntValue(ACE_INT32 thrId, ACE_INT32 opt, ACE_INT32 value);
		
		std::string checkStrValue(ACE_INT32 thrId, ACE_INT32 opt, std::string value);
		
		bool createRescueDirectory();
		
		bool writeToRescueFile();
		
		bool recoverFromRescueFile();
		
		ACE_INT32 deleteRescueFile();

		std::string currentDirectory();

		bool clearDirectory(std::string dir);

		void errorText();
};
#endif