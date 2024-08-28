#ifndef AES_AFP_TRANSFERENTITY_OBJECTIMPLEMENTER_H
#define AES_AFP_TRANSFERENTITY_OBJECTIMPLEMENTER_H
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */

#if 0

#include <saImmOi.h>
#include <saImm.h>

#include <poll.h>
#include <ace/ACE.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ACS_CC_Types.h>
#include <aes_afp_datatask.h>
#include <ACS_APGCC_RuntimeOwner_V2.h>
#include <aes_afp_datablock.h>

#define AES_AFP_TRANSFERENTITYIMPLEMENTER 	"TransferEntityImpl"
#define	AES_AFP_TRANSFER_ENTITY_ID 			"transferEntityInfoId"
#define AES_CDH_RESPONDING_DESTSET_RDN		"respondingDestinationSetId"
#define	AES_CDH_RESPONDING_DESTSET_CLASS_NM	"AxeDataTransferRespondingDestinationSet"
#define	AES_AFP_FILE_TRANSMISSION_STATUS	"transmissionStatus"
#define	AES_AFP_IS_DIR_FLAG					"isDirectory"
#define AES_CDH_INITIATING_DESTSET_RDN		"initiatingDestinationSetId"
#define	AES_CDH_INITIATING_DESTSET_CLASS_NM	"AxeDataTransferInitiatingDestinationSet"
#define	AES_AFP_TRANSFER_ENTITY_CLASS_NM	"AxeDataTransferTransferEntityInfo"
#define AES_AFP_FILE_TRANSFER_QUEUE_CLASS_NM	"AxeDataTransferFileTransferQueue"
#define AES_AFP_FILE_TRANSFER_QUEUE_RDN			"fileTransferQueueId"

typedef map<string, string> transferEntityMapType;
typedef pair<string, string> transferEntityPair;

class aes_afp_transfer_entity : public ACS_APGCC_RuntimeOwner_V2
{
	/*=====================================================================
                           PUBLIC DECLARATION SECTION
         ==================================================================== */
public:
	/** @brief aes_afp_transfer_entity constructor
	 *
	 *      aes_afp_transfer_entity Constructor of class
	 *
	 *      @remarks Remarks
	 */
	aes_afp_transfer_entity(aes_afp_datatask* dataThrd);

	/** @brief aes_afp_transfer_entity Destructor
	 *
	 *      aes_afp_transfer_entity Destructor of class
	 *
	 *      @remarks Remarks
	 */
	~aes_afp_transfer_entity();
	/**
	 * @brief updateCallback method
	 * updateCallback method: This is a base class method.
	 * This method will be called as a callback when an non-cached attribute of a runtime Object
	 * is updated.
	 *
	 * @param  objName:   the distinguished name of the object for which the update is requested.
	 *
	 * @param  attrName:  the name of the attribute for which values must be updated.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	ACS_CC_ReturnType updateCallback(const char* objName, const char** attrName);

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
				ACS_APGCC_InvocationType invocation,
				const char* p_objName,
				ACS_APGCC_AdminOperationIdType operationId,
				ACS_APGCC_AdminOperationParamType**paramList);

	bool initTransferEntityRunTimeOwner();

	bool finalizeTransferEntityRunTimeOwner();

	ACS_CC_ReturnType createTransferEntityInfo(std::string& destionationSetName,
						   std::string& transferQueueName,
						    std::string& fileName);

	ACS_CC_ReturnType deleteTransferEntity(const std::string& aFileName,
					const std::string& aDestinationSetName,
					const std::string& aTransferQueueName);
	
	ACS_CC_ReturnType getTQDestSetFileInfo( const char *aOBJName,
						 std::string &transferQueue,
						 std::string &destinationSet,
						 std::string &fileName);

private:
	aes_afp_datatask* dataThrd;
	transferEntityMapType transferEntityMap;
	
	
};

class aes_afp_transferEntity_runtimeHandlerThread : public ACE_Task_Base
{
public :

	/** @brief aes_afp_transferEntity_runtimeHandlerThread constructor
	 *
	 *      aes_afp_transferEntity_runtimeHandlerThread Constructor of class
	 *
	 *      @remarks Remarks
	 */
	aes_afp_transferEntity_runtimeHandlerThread(ACE_Thread_Manager* threadManager_);

	/** @brief aes_afp_transferEntity_runtimeHandlerThread constructor for initializing Implementer
	 *
	 *      aes_afp_transferEntity_runtimeHandlerThread Constructor of class
	 *
	 *      @remarks Remarks
	 */
	aes_afp_transferEntity_runtimeHandlerThread(aes_afp_transfer_entity *pImpl);

	/** @brief aes_afp_transferEntity_runtimeHandlerThread destructor
	 *
	 *      aes_afp_transferEntity_runtimeHandlerThread destructor of class
	 *
	 *      @remarks Remarks
	 */
	~aes_afp_transferEntity_runtimeHandlerThread();
	/** @brief setImpl method
	 *
	 *      setImpl for setting implementer
	 *
	 *      @remarks Remarks
	 */
	void setImpl(aes_afp_transfer_entity *pImpl);

	/** @brief stop method
	 *
	 *      stop for stopping the thread
	 *
	 *      @remarks Remarks
	 */
	void stop();

	//svc method
	int svc(void);


private:

	aes_afp_transfer_entity *theTransferEntityRuntimeOwnerImplementer;
	bool theIsStop;


};
// aes_afp_transferEntity_runtimeHandlerThread END
#endif
#endif



