
#ifndef aes_afp_file_h
#define aes_afp_file_h 1

#include <aes_gcc_filestates.h>
#include <aes_gcc_filestates_r1.h>
#include <aes_gcc_format.h>
#include "aes_afp_datablock.h"
#include "aes_afp_msgtypes.h"
#include "aes_afp_parameter.h"
#include "aes_afp_timestamp.h"
#include "aes_afp_protocol.h"
#include <fstream>
#include <string>
#include <sstream>

#include <ace/ACE.h>

// Must be last
#include <aes_gcc_log.h>
//## Class: aes_afp_file%370AEDB300F3
//	This class is used for meta data about files.
//	Trace point: aes_afp_file traces file activity, status
//	changes and so on.

class aes_afp_file 
{
  public:
    //## Constructors (generated)
      aes_afp_file();

    //## Destructor (generated)
      virtual ~aes_afp_file();


    //## Other Operations (specified)
      //## Operation: init%371478C80309; C++
      //	Initiates the file.
	  //HI70260 - REMOVEDELAY attribute has been moved to destinationset level, 
	  //and aes_afp_file dont have removedelay attribute. Hence removeDelay has been removed from argument list
      bool init (std::string tempDiscName, std::string afpDiscName, AES_GCC_Filestates status, AES_GCC_Errorcodes& error,std::string sourceDirPath, bool removeBefore = false, bool isDirectory = false, std::string originalFilename="", std::string transferMask = "*.*", AES_GCC_Format transferMode = AES_BINARY);

      //	Initiates the object from a file.
      bool initFromFile (std::string path, AES_GCC_Errorcodes& error);

      //	Set the status of a file.
      bool setStatus (std::string& tempDiscName, std::string& afpDiscName, AES_GCC_Filestates status, AES_GCC_Errorcodes& error);

      //## Operation: getStatus%370B55B500C7
      //	Retreive the status for a send item.
      AES_GCC_Filestates getStatus ()const;

      //## Operation: removeFile%37170EB302EA; C++
      //	Removes a file from afp.
      bool removeFile (std::string& afpDiscName, AES_GCC_Errorcodes& error);

      //## Operation: fillFileInfo%375E409D019A; C++
      //	Retreives information from database and fills the
      //	supplied strstream buffer with the data.
    
	  //HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
	  //and it will used for further calculations
	  void fillFileInfo (std::string& listBuffer, int removeDelay);

      //## Operation: readyForDelete%3764DDF702B4; C++
      //	Deletes a send item that is ready for delition.
	  //HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
	  //and it will used for further calculations to delete the file
      bool readyForDelete (AES_GCC_Errorcodes& error, int removeDelay);

      //## Operation: getFile%37650211032C; C++
      //	Fills a datablock with information for the file.
      void getFile (aes_afp_datablock* dBlock);

      //## Operation: getCreationDate%37E48F3202B0; C++
      //	Returns the time when the file was created.
      std::string getCreationDate ()const;

      //## Operation: nextSeqNrCreationDate; C++
      //	Returns the time when the file was created,
	  //	with a new seqNr (000-999).
      std::string nextSeqNrCreationDate ();	
      //##Operation: getOriginalFileName; C++
      //        Returns the originalfilename  when requested.
          //    return the string .
      std::string getOriginalFileName();     //HU96961
      //## Operation: getDirectoryFlag%3B3EF01103D1; C++
      //	Flag is set to true if transferring a directory,
      //	otherwise false.
      
      bool getDirectoryFlag ()const;

      //## Operation: getTransferMask%3B3EF0220372; C++
      //	Retrieves file mask for files in a transfer queue. The
      //	file mask is used when transferring directories.
      std::string getTransferMask ()const;

      //## Operation: getTransferMode%3B3F02FF00AE; C++
      //	Retrieves transfer format for a transfer queues files.
      AES_GCC_Format getTransferMode ()const;
	std::string getSourceDirPath()const;

      //## Operation: isYounger; C++
      //	Returns true if this is younger than fileObj.
      bool isYounger(aes_afp_file & fileObj);

	   // added by deb
	  void recover_data();
	  bool dumpToFile_new (std::string& tempPath, AES_GCC_Errorcodes& error);
	  bool isFileStatusDataValid();
					  
	  // Additional Public Declarations
      //## begin aes_afp_file%370AEDB300F3.public preserve=yes
      //## end aes_afp_file%370AEDB300F3.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_file%370AEDB300F3.protected preserve=yes
      //## end aes_afp_file%370AEDB300F3.protected

  private:

    //## Other Operations (specified)
      //## Operation: calcRemDelay%3760EE100101; C++
      //	Calculates the remaining time before this send item is
      //	deleted.
	  //HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
	  //and it will used for further calculations to delete the file
      ACE_INT64 calcRemDelay (ACE_INT32 removeDelay);

      //## Operation: dumpToFile%3E4BAD55008A; C++
      //	Dump data to file.
      bool dumpToFile (std::string& tempPath, std::string& path, AES_GCC_Errorcodes& error);

    // Additional Private Declarations
      //## begin aes_afp_file%370AEDB300F3.private preserve=yes
      //## end aes_afp_file%370AEDB300F3.private
	  	  //For TR HK91965
	  bool doDirDelete(std::string path);


  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: creationDate%3E50E7C20347
      //## begin aes_afp_file::creationDate%3E50E7C20347.attr preserve=no  private: char[18] {U} 
      char creationDate_[21];
      //## end aes_afp_file::creationDate%3E50E7C20347.attr

      //## Attribute: readyDate%3E50E7D800E6
      //## begin aes_afp_file::readyDate%3E50E7D800E6.attr preserve=no  private: char[18] {U} 
      char readyDate_[18];
      //## end aes_afp_file::readyDate%3E50E7D800E6.attr

      //## Attribute: sendDate%3E50E7E9028F
      //## begin aes_afp_file::sendDate%3E50E7E9028F.attr preserve=no  private: char[18] {U} 
      char sendDate_[18];
      //## end aes_afp_file::sendDate%3E50E7E9028F.attr

      //## Attribute: failedDate%3E50E7F80074
      //## begin aes_afp_file::failedDate%3E50E7F80074.attr preserve=no  private: char[18] {U} 
      char failedDate_[18];
      //## end aes_afp_file::failedDate%3E50E7F80074.attr

      //## Attribute: deleteDate%3E50E80200C8
      //## begin aes_afp_file::deleteDate%3E50E80200C8.attr preserve=no  private: char[18] {U} 
      char deleteDate_[18];
      //## end aes_afp_file::deleteDate%3E50E80200C8.attr

      //## Attribute: noneDate%3E50E80E0043
      //## begin aes_afp_file::noneDate%3E50E80E0043.attr preserve=no  private: char[18] {U} 
      char noneDate_[18];
      //## end aes_afp_file::noneDate%3E50E80E0043.attr

      //## Attribute: archiveDate%3E50E8180070
      //## begin aes_afp_file::archiveDate%3E50E8180070.attr preserve=no  private: char[18] {U} 
      char archiveDate_[18];
      //## end aes_afp_file::archiveDate%3E50E8180070.attr

      //## Attribute: stoppedDate%3E51E7400057
      //## begin aes_afp_file::stoppedDate%3E51E7400057.attr preserve=no  private: char[18] {U} 
      char stoppedDate_[18];
      //## end aes_afp_file::stoppedDate%3E51E7400057.attr

      //## Attribute: absoluteCreationTime%3E50E8220038
      //## begin aes_afp_file::absoluteCreationTime%3E50E8220038.attr preserve=no  private: ACE_UINT64 {U} 0
      ACE_UINT64 absoluteCreationTime_;
      //## end aes_afp_file::absoluteCreationTime%3E50E8220038.attr

      //## Attribute: absoluteDeletionTime%3E50E8370060
      //## begin aes_afp_file::absoluteDeletionTime%3E50E8370060.attr preserve=no  private: ACE_UINT64 {U} 0
      ACE_UINT64 absoluteDeletionTime_;
      //## end aes_afp_file::absoluteDeletionTime%3E50E8370060.attr

      //## Attribute: isDirectory%3E50E84A0298
      //## begin aes_afp_file::isDirectory%3E50E84A0298.attr preserve=no  private: bool {U} false
      bool isDirectory_;
      //## end aes_afp_file::isDirectory%3E50E84A0298.attr

      //## Attribute: transferMode%3E50E85600FB
      //## begin aes_afp_file::transferMode%3E50E85600FB.attr preserve=no  private: AES_GCC_Format {U} AES_BINARY
      AES_GCC_Format transferMode_;
      //## end aes_afp_file::transferMode%3E50E85600FB.attr

      //## Attribute: transferMask%3E50E86E0286
      //## begin aes_afp_file::transferMask%3E50E86E0286.attr preserve=no  private: char[8] {U} 
      // char transferMask_[8]; jost
	  std::string transferMask_;
      //## end aes_afp_file::transferMask%3E50E86E0286.attr

      //## Attribute: status%3E50EA790025
      //## begin aes_afp_file::status%3E50EA790025.attr preserve=no  private: AES_GCC_Filestates {U} AES_FSREADY

      AES_GCC_Filestates status_;
      //## end aes_afp_file::status%3E50EA790025.attr

	//## Attribute: originalFileName%3E50EAC20247    //HU96961
      //## begin aes_afp_file::originalFileName_%3E50EAC20247.attr preserve=no  private: char[8] {U}
      std::string originalFileName_;
      //## end aes_afp_file::originalFileName_%3E50EAC20247.attr


      //## Attribute: removeBefore%3E50EAC20247
      //## begin aes_afp_file::removeBefore%3E50EAC20247.attr preserve=no  private: bool {U} false
      bool removeBefore_;
      //## end aes_afp_file::removeBefore%3E50EAC20247.attr

      //## Attribute: removeDelay%3E50EACE03B7
      //## begin aes_afp_file::removeDelay%3E50EACE03B7.attr preserve=no  private: int {U} 0
	  //HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
	  //and it will used for further calculations
      //int removeDelay_;	
      //## end aes_afp_file::removeDelay%3E50EACE03B7.attr

	  //## Attribute: sequenceCounter
      //## begin aes_afp_file::sequenceCounter.attr preserve=no  private: int {U} 0
      int sequenceCounter_;
	  //## end aes_afp_file::sequenceCounter.attr
	std::string sourceDir_; 
    
	  // Additional Implementation Declarations
      //## begin aes_afp_file%370AEDB300F3.implementation preserve=yes
      //## end aes_afp_file%370AEDB300F3.implementation

};

//## begin aes_afp_file%370AEDB300F3.postscript preserve=yes
//## end aes_afp_file%370AEDB300F3.postscript

// Class aes_afp_file 

//## begin module%370AEEB20027.epilog preserve=yes
//## end module%370AEEB20027.epilog


#endif
