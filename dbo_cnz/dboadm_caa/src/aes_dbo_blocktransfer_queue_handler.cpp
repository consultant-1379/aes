/*=================================================================== */
/**
   @file aes_dbo_blocktransfer_queue_handler.cpp

   Class method implementation for DBO module.

   This module contains the implementation of class declared in
   the aes_dbo_blocktransfer_queue_handler.h file.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/08/2012   XTANAGG   Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_dbo_blocktransfer_queue_handler.h>
#include <aes_dbo_opentqsessions.h>
#include <aes_dbo_server.h>
#include <aes_dbo_tqdatabase.h>
#include <AES_DBO_TQManager.h>
#include <aes_cdh_destinationset.h>
#include <acs_apgcc_omhandler.h>
#include <ACS_APGCC_Util.H>
#include <aes_gcc_tracer.h>
#include <aes_gcc_util.h>
#include <ace/Reactor.h>


PrevState *prevState = 0;
GCC_TDEF(AES_DBO_block_transferq_handler);
AES_GCC_TRACE_DEFINE(AES_DBO_block_transferq_handler)

/*===================================================================
   ROUTINE: AES_DBO_BlockTransferQueueCmdHandler
=================================================================== */
AES_DBO_BlockTransferQueueCmdHandler::AES_DBO_BlockTransferQueueCmdHandler(std::string aBlockTransferQueueClassName, std::string aBlockTransferQueueImplName, ACS_APGCC_ScopeT p_scope):
acs_apgcc_objectimplementerinterface_V3(aBlockTransferQueueClassName, aBlockTransferQueueImplName, p_scope),
m_poOiHandler(),
theClassName(aBlockTransferQueueClassName)
{
	m_isClassImplAdded = false;
	m_poThreadManager = 0;
}//End of Constructor

/*===================================================================
   ROUTINE: setObjectImplementer
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferQueueCmdHandler::setObjectImplementer()
{
	ACS_CC_ReturnType errorCode = ACS_CC_FAILURE;
	AES_GCC_TRACE_MESSAGE("Entering setObjectImplementer");

	for(int i=0; i < 10; i++)
	{
		errorCode = m_poOiHandler.addClassImpl(this, theClassName.c_str());
		if( errorCode == ACS_CC_FAILURE )
		{
			GCC_TERROR(( AES_DBO_block_transferq_handler,"(%t) AES_DBO_BlockTransferQueueCmdHandler::setObjectImplementer: Error occurred while add class implementer for Destination\n"));
			int intErr = getInternalLastError();
			if ( intErr == -6)
			{
				ACE_OS::sleep(1);
				continue;
			}
			else
			{
				GCC_TERROR(( AES_DBO_block_transferq_handler,"(%t) AES_DBO_BlockTransferQueueCmdHandler::setObjectImplementer: Error occurred while add class implementer for Destination, ErrCode = %d\n", intErr));
				break;
			}
		}
		else
		{
			m_isClassImplAdded = true;
			break;
		}
	}
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving setObjectImplementer\n"));
	return errorCode;
}//End of setObjectImplementer

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferQueueCmdHandler::create(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *className,
		const char* parentName,
		ACS_APGCC_AttrValues **attr)
{
        (void) className;
	GCC_TTRACE (( AES_DBO_block_transferq_handler, "(%t) Entering AES_DBO_BlockTransferQueueCmdHandler::create\n"));
	(void) oiHandle;

	string objName;

	int i = 0;
	char myBlockTransferQRdn[AES_DBO_IMM_DN_PATH_SIZE_MAX] = {0};
	string myBlockTQName;
	ACE_INT32 mirrored = 0;
	ACE_INT32 removeDelay = 1440;
	string blockDestSet;

	//Parse the arguments

	int ctr = 0;
	while( attr[ctr] )
	{
		if ( ACE_OS::strcmp(attr[ctr]->attrName, AES_DBO_BLOCK_TRANSFER_QUEUE_RDN) == 0 )
		{
			::strncpy(myBlockTransferQRdn, (reinterpret_cast<char *>(attr[ctr]->attrValues[0])),AES_DBO_IMM_DN_PATH_SIZE_MAX);
			GCC_TDEBUG ((AES_DBO_block_transferq_handler,"(%t) RDN = %s\n", myBlockTransferQRdn));
		}
		ctr++;
	}
	
	//Extract the name of the block transfer queue from the RDN.
	
	extractTQName(myBlockTransferQRdn, "RDN", myBlockTQName);
	char objNm [AES_DBO_IMM_DN_PATH_SIZE_MAX]= {0};
	::snprintf(objNm, AES_DBO_IMM_DN_PATH_SIZE_MAX,"%s,%s",myBlockTransferQRdn, parentName);

	//Validate the name of block transfer queue.
	if( validTQName( myBlockTQName )  == false )
	{
		GCC_TDEBUG (( AES_DBO_block_transferq_handler,"(%t) The value of block transfer queue is not valid\n"));
		GCC_TTRACE (( AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
		//set error code for invalid value
		setErrorCode(AES_UNREASVAL_BLOCKTQNAME, objNm);
		return ACS_CC_FAILURE;
	}

	
	while( attr[i] )
	{
		if( ACE_OS::strcmp(attr[i]->attrName, AES_DBO_MIRRORED_ATTR) == 0 )
		{
			 if( attr[i]->attrValuesNum != 0 )
			 {
				 mirrored = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
				 GCC_TDEBUG ((AES_DBO_block_transferq_handler,"(%t) Mirrored = %d\n", mirrored));

				 if( mirrored < 0 || mirrored > 1 )
				 {
					 GCC_TDEBUG ((AES_DBO_block_transferq_handler,"(%t) Value of mirrored is out of range\n"));
					 GCC_TTRACE ((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
					 //set error code for invalid value
					 setErrorCode(AES_UNREASVAL_BLOCKMIRROR, objNm);
					 return ACS_CC_FAILURE;
				 }
			 }
		}
		else if( ACE_OS::strcmp(attr[i]->attrName, AES_DBO_REMOVE_DELAY_ATTR) == 0 )
		{
			if( attr[i]->attrValuesNum != 0 )
			{
				removeDelay = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
				GCC_TDEBUG ((AES_DBO_block_transferq_handler,"(%t) removeDelay = %d\n", removeDelay));
				if( removeDelay < 1 || removeDelay > 10080 )
				{
					GCC_TDEBUG ((AES_DBO_block_transferq_handler,"(%t) Value of remove Delay is out of range\n"));
					GCC_TTRACE ((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
					//set error code for invalid value
					setErrorCode(AES_UNREASVAL_BLOCKREMDELAY, objNm);
					return ACS_CC_FAILURE;
				 }
			}
		}
		else if( ACE_OS::strcmp(attr[i]->attrName, AES_DBO_BLOCK_DESTINATION_SET_ATTR) == 0 )
		{
			if( attr[i]->attrValuesNum != 0 )
			{
				blockDestSet = reinterpret_cast<char *>(attr[i]->attrValues[0]);
				GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) Block Destination Set = %s\n", blockDestSet.c_str()));
			}
			else
			{
			 	GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) The value of block destination set is not specified by the user\n"));
				GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
				//set error code for invalid value
				setExitCode(2, "Incorrect Usage");
				return ACS_CC_FAILURE;
			}
		}
		i++;
	}

	if( validDestSetName( blockDestSet ) == false )
	{
			GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) The value of block destination set is not valid\n"));
			GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
			//set error code for invalid value
			setErrorCode(AES_UNREASVAL_BLOCKDESTSET, objNm);
			return ACS_CC_FAILURE;
	}

	AES_CDH_DestinationSet d(blockDestSet);
	unsigned int rCode = d.exists(AES_CDH_DestinationSet::BLOCK);


	if (rCode != AES_CDH_RC_OK)
	{
		switch (rCode)
		{
		case AES_CDH_RC_NODESTSET:
		    		setErrorCode(AES_NODESTINATIONSET);
				GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
				return ACS_CC_FAILURE;
		    		break;

		case AES_CDH_RC_NOSERVER:
		    		setErrorCode(AES_NOCDHSERVER);
				GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
				return ACS_CC_FAILURE;
			    	break;

		case AES_CDH_RC_NOTBLOCKDESTSET:
		    		setErrorCode(AES_INVALIDTRANSTYPE);
				GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
				return ACS_CC_FAILURE;
		    		break;
		}
        }
	//Create the object name.
	objName = myBlockTransferQRdn;
	objName += ",";
	objName += parentName;

	int resultCode = aes_dbo_tqdatabase::instance()->defineTQ(myBlockTQName, blockDestSet, mirrored, removeDelay);
	if( resultCode != AES_NOERRORCODE )
	{
		setErrorCode( resultCode );
		GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
		return ACS_CC_FAILURE;
	}

	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));

	//Now update the RO attribute
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;

	ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );
	if (retVal != ACS_CC_SUCCESS)
	{
		setErrorCode(AES_CATASTROPHIC);
		//TO DO deletion for tq
		aes_dbo_tqdatabase::instance()->deleteTQ(myBlockTQName);
		GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) getCcbAugmentationInitialize::ACS_CC_FAILURE\n"));
		GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
		return ACS_CC_FAILURE;
	}
	retVal = updateRefAttibutes(ccbHandleVal,
				adminOwnwrHandlerVal,
				objName,
				AES_DBO_BLOCK_DESTINATION_SET_RO_ATTR,
				blockDestSet);
	if( retVal != ACS_CC_SUCCESS )
	{
		setErrorCode(AES_CATASTROPHIC);
		aes_dbo_tqdatabase::instance()->deleteTQ(myBlockTQName);
		GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) updateRefAttibutes::ACS_CC_FAILURE\n"));
		GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
		return ACS_CC_FAILURE;
	}

	retVal = applyAugmentCcb (ccbHandleVal);
	if (retVal != ACS_CC_SUCCESS)
	{
		setErrorCode(AES_CATASTROPHIC);
		aes_dbo_tqdatabase::instance()->deleteTQ(myBlockTQName);
		GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) applyAugmentCcb::ACS_CC_FAILURE\n"));
		GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
		return ACS_CC_FAILURE;
	}

	//Handle intermediate abort
	prevState = new (std::nothrow) PrevState;
	if(prevState == 0)
	{
		GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) AES_DBO_BlockTransferQueueCmdHandler::create mem allocation failed for prevState\n"));
	}
	else
	{
		prevState->tqData.tq = myBlockTQName;
		prevState->tqData.dest = blockDestSet;
		prevState->tqData.mirror = mirrored;
		prevState->tqData.delay = removeDelay;
		prevState->undoAction = IMMC_REMOVE_TQ;
	}
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::create\n"));
	setErrorCode(AES_NOERRORCODE);
	return ACS_CC_SUCCESS;
}

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferQueueCmdHandler::deleted( ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *objName)
{
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering AES_DBO_BlockTransferQueueCmdHandler::deleted\n"));
	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	string myBlockTQName;

	extractTQName(objName, "OBJNAME",  myBlockTQName);

	GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) Delete request came for TQ %s\n", myBlockTQName.c_str()));

	//Handle intermediate abort
	prevState = new (std::nothrow) PrevState;
	if(prevState == 0)
	{
		GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) AES_DBO_BlockTransferQueueCmdHandler::deleted mem allocation failed for prevState\n"));
	}
	else
	{
		prevState->tqData.tq = myBlockTQName;
		if(aes_dbo_tqdatabase::instance()->getTqDetails(myBlockTQName,prevState->tqData.dest,prevState->tqData.mirror,prevState->tqData.delay))
			GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) AES_DBO_BlockTransferQueueCmdHandler::deleted fetched tqDetails present to store in prevState tq=%s, dest=%s, mirror=%d, delay=%d\n",prevState->tqData.tq.c_str(),prevState->tqData.dest.c_str(),prevState->tqData.mirror,prevState->tqData.delay));
		
		prevState->undoAction = IMMC_CREATE_TQ;
	}

	int resultCode = aes_dbo_tqdatabase::instance()->deleteTQ(myBlockTQName);
	setErrorCode( resultCode );
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::delete with error code %d\n", resultCode ));
	if( resultCode != AES_NOERRORCODE )
	{
		return ACS_CC_FAILURE;
	}

	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::deleted\n"));
	return ACS_CC_SUCCESS;
}//End of deleted

/*===================================================================
ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferQueueCmdHandler::modify(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *objName,
		ACS_APGCC_AttrModification **attrMods)
{
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering AES_DBO_BlockTransferQueueCmdHandler::modify\n"));
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	int i = 0;

	ACE_INT32 newMirrored = 0;
	ACE_INT32 oldMirrored = 0;
	ACE_INT32 mirrored    = 0;

	ACE_INT32 newRemoveDelay = 1440;
	ACE_INT32 oldRemoveDelay = 1440;
	ACE_INT32 removeDelay    = 1440;

	std::string newBlockDestSet;
	std::string oldBlockDestSet;
	std::string blockDestSet;

	bool isMirroringSpecified = false;
	bool isRemoveDelaySpecified = false;
	bool isBlockDestSetSpecified = false;

	while( attrMods[i] )
	{
		if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_DBO_MIRRORED_ATTR) == 0 )
		{
			isMirroringSpecified = true;
			if( attrMods[i]->modAttr.attrValuesNum != 0 )
			{
				newMirrored = *reinterpret_cast<ACE_INT32 *>(attrMods[i]->modAttr.attrValues[0]);
				if( newMirrored < 0 || newMirrored > 1 )
				{
					GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) Value of mirrored is out of range\n"));
					GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify\n"));

					//set error code for invalid value
					setErrorCode(AES_UNREASVAL_BLOCKMIRROR, objName);
					return ACS_CC_FAILURE;
				}
			}
		}
		else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_DBO_REMOVE_DELAY_ATTR) == 0 )
		{
			isRemoveDelaySpecified = true;
			if(attrMods[i]->modAttr.attrValuesNum !=0 )
			{
				newRemoveDelay = *reinterpret_cast<ACE_INT32 *>(attrMods[i]->modAttr.attrValues[0]);
				if( newRemoveDelay < 1 || newRemoveDelay > 10080 )
				{
					GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) Value of remove Delay is out of range\n"));
					GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify\n"));
					//set error code for invalid value
					setErrorCode(AES_UNREASVAL_BLOCKREMDELAY, objName);
					return ACS_CC_FAILURE;
				 }
			}
		}
		else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_DBO_BLOCK_DESTINATION_SET_ATTR) == 0 )
		{
			isBlockDestSetSpecified = true;
			if(attrMods[i]->modAttr.attrValuesNum !=0 )
			{
				newBlockDestSet = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
				if( !newBlockDestSet.empty())
				{
					if( validDestSetName( newBlockDestSet ) == false )
					{
							GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) The value of block destination set is not valid\n"));
							GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify\n"));
							//set error code for invalid value
							setErrorCode(AES_UNREASVAL_BLOCKDESTSET, objName);
							return ACS_CC_FAILURE;
					}
					AES_CDH_DestinationSet d(newBlockDestSet);
        				unsigned int rCode = d.exists(AES_CDH_DestinationSet::BLOCK);

					if (rCode != AES_CDH_RC_OK)
					{
						switch (rCode)
						{
						case AES_CDH_RC_NODESTSET:
								setErrorCode(AES_NODESTINATIONSET);
								GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify AES_NODESTINATIONSET\n"));
								return ACS_CC_FAILURE;
								break;

						case AES_CDH_RC_NOSERVER:
								setErrorCode(AES_NOCDHSERVER);
								GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify AES_NOCDHSERVER\n"));
								return ACS_CC_FAILURE;
								break;

						case AES_CDH_RC_NOTBLOCKDESTSET:
								setErrorCode(AES_INVALIDTRANSTYPE);
								GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify AES_CDH_RC_NOTBLOCKDESTSET\n"));
								return ACS_CC_FAILURE;
								break;
						}
					}
				}
			}
		}
		i++;
	}

	string myBlockTQName;
	extractTQName(objName, "OBJNAME",  myBlockTQName);

	//Get the current Transfer queue attributes.
	aes_dbo_tqdatabase::instance()->getRemoveDelay(myBlockTQName, oldRemoveDelay );
	GCC_TDEBUG((AES_DBO_block_transferq_handler,"The value of remove delay fetched oldRemoveDelay: %d\n", oldRemoveDelay));

	oldMirrored =  aes_dbo_tqdatabase::instance()->mirrored( myBlockTQName);
	GCC_TDEBUG((AES_DBO_block_transferq_handler,"The value of remove delay fetched mirrored: %d\n", oldMirrored));

	aes_dbo_tqdatabase::instance()->getDestinationSet( myBlockTQName, oldBlockDestSet);
	
	if( isMirroringSpecified )
	{
		GCC_TDEBUG((AES_DBO_block_transferq_handler,"isMirroringSpecified true\n"));
		mirrored = newMirrored;
	}
	else
	{
		GCC_TDEBUG((AES_DBO_block_transferq_handler,"isMirroringSpecified false\n"));
		mirrored = oldMirrored;
	}

	if( isRemoveDelaySpecified )
	{
		GCC_TDEBUG((AES_DBO_block_transferq_handler,"isRemoveDelaySpecified true\n"));
		removeDelay = newRemoveDelay;
	}
	else
	{
		GCC_TDEBUG((AES_DBO_block_transferq_handler,"isRemoveDelaySpecified false\n"));
		removeDelay = oldRemoveDelay;
	}

	if( isBlockDestSetSpecified )
	{
		GCC_TDEBUG((AES_DBO_block_transferq_handler,"isBlockDestSetSpecified true\n"));
		blockDestSet = newBlockDestSet;
	}
	else
	{
		GCC_TDEBUG((AES_DBO_block_transferq_handler,"isBlockDestSetSpecified false\n"));
		blockDestSet = oldBlockDestSet;
	}

	if( blockDestSet.empty() )
	{
		GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) The value of block destination set is empty\n"));
		GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify\n"));
		//set error code for invalid value
		setExitCode(2, "Incorrect Usage");
		return ACS_CC_FAILURE;
	}

	//Handle intermediate abort
	prevState = new (std::nothrow) PrevState;
	if(prevState == 0)
	{
		GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) AES_DBO_BlockTransferQueueCmdHandler::modify mem allocation failed for prevState\n"));
	}
	else
	{
		prevState->tqData.tq = myBlockTQName;
		prevState->tqData.dest = oldBlockDestSet;
		prevState->tqData.mirror = oldMirrored;
		prevState->tqData.delay = oldRemoveDelay;
		prevState->undoAction = IMMC_MODIFY_TQ;
		GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) AES_DBO_BlockTransferQueueCmdHandler::modify revert to old values tq=%s, dest =%s, mirror=%d, delay=%d", prevState->tqData.tq.c_str(), prevState->tqData.dest.c_str(), prevState->tqData.mirror, prevState->tqData.delay));
	}

	unsigned int resultCode = AES_NOERRORCODE;

	if (isBlockDestSetSpecified && !isMirroringSpecified )
	{
		resultCode = aes_dbo_tqdatabase::instance()->changeTQDest(myBlockTQName, blockDestSet);
	}
	else if (!isBlockDestSetSpecified && isMirroringSpecified )
	{
		resultCode = aes_dbo_tqdatabase::instance()->changeTQMirror(myBlockTQName, mirrored);
	}
	else if (isBlockDestSetSpecified && isMirroringSpecified )
	{
		resultCode = aes_dbo_tqdatabase::instance()->changeTQ(myBlockTQName, blockDestSet, mirrored, removeDelay);
	}

	//Set the error code
	setErrorCode( resultCode );

	if(resultCode == AES_NOERRORCODE)
	{
		if(isMirroringSpecified || isRemoveDelaySpecified)
		{
			aes_dbo_tqdatabase::instance()->changeTQDelay(myBlockTQName, removeDelay);

			// Get the active tq-session
			tqSession_ = aes_dbo_opentqsessions::instance()->exists(myBlockTQName.c_str());
			if (tqSession_)
			{
				// Change the necessary parameters
				tqSession_->changeMirror(mirrored);
				tqSession_->changeDelay(removeDelay);
			}
		}
	}

	GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify with error code %d\n", resultCode));
	if( resultCode != AES_NOERRORCODE )
	{
		return ACS_CC_FAILURE;
	}

	if( isBlockDestSetSpecified )
	{
		//Now update the RO attribute
		ACS_APGCC_CcbHandle ccbHandleVal;
		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;

		ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );
		if (retVal != ACS_CC_SUCCESS)
		{
			setErrorCode(AES_CATASTROPHIC);
			GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) getCcbAugmentationInitialize::ACS_CC_FAILURE\n"));
			GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify\n"));
			return ACS_CC_FAILURE;
		}
		retVal = updateRefAttibutes(ccbHandleVal,
					adminOwnwrHandlerVal,
					objName,
					AES_DBO_BLOCK_DESTINATION_SET_RO_ATTR,
					blockDestSet);
		if( retVal != ACS_CC_SUCCESS )
		{
			setErrorCode(AES_CATASTROPHIC);
			GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) getCcbAugmentationInitialize::ACS_CC_FAILURE\n"));
			GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify\n"));
			return ACS_CC_FAILURE;
		}

		retVal = applyAugmentCcb (ccbHandleVal);
		if (retVal != ACS_CC_SUCCESS)
		{
			setErrorCode(AES_CATASTROPHIC);
			GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) applyAugmentCcb::ACS_CC_FAILURE\n"));
			GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify\n"));
			return ACS_CC_FAILURE;
		}
	}
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::modify\n"));
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferQueueCmdHandler::complete( ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering AES_DBO_BlockTransferQueueCmdHandler::complete\n"));
	(void)oiHandle;
	(void)ccbId;
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::complete\n"));
	if(prevState!=0)
	{
		delete prevState;
		prevState = 0;
	}
	return ACS_CC_SUCCESS;

} //End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void AES_DBO_BlockTransferQueueCmdHandler::abort(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering AES_DBO_BlockTransferQueueCmdHandler::abort\n"));
	(void)oiHandle;
	(void)ccbId;
	if(prevState!=0)
	{
		switch(prevState->undoAction)
		{
			int resultCode;
			case IMMC_REMOVE_TQ:
			{
				resultCode = aes_dbo_tqdatabase::instance()->deleteTQ(prevState->tqData.tq);
				GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) Leaving Abort:IMMC_REMOVE_TQ with error code %d\n", resultCode));
			}
				break;
			case IMMC_CREATE_TQ:
			{
				resultCode = aes_dbo_tqdatabase::instance()->defineTQ(prevState->tqData.tq, prevState->tqData.dest, prevState->tqData.mirror, prevState->tqData.delay);
				GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) Leaving Abort: IMMC_CREATE_TQ with error code %d\n", resultCode));
			}
				break;
			case IMMC_MODIFY_TQ:
			{
				resultCode = aes_dbo_tqdatabase::instance()->changeTQ(prevState->tqData.tq, prevState->tqData.dest, prevState->tqData.mirror, prevState->tqData.delay);

				if(resultCode == AES_NOERRORCODE)
				{
					// Get the active tq-session
					tqSession_ = aes_dbo_opentqsessions::instance()->exists(prevState->tqData.tq.c_str());
					if (tqSession_)
					{
						// Change the necessary parameters
						tqSession_->changeMirror(prevState->tqData.mirror);
						tqSession_->changeDelay(prevState->tqData.delay);
					}
				}

				GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) Abort:IMMC_MODIFY_TQ with error code %d\n", resultCode));
			}
				break;
			default:
				break;
		}
		delete prevState;
		prevState = 0;
	}
}//End of abort

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferQueueCmdHandler::updateRuntime( const char* objName,
		const char** attrName)
{
	(void)objName;
	(void)attrName;
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void AES_DBO_BlockTransferQueueCmdHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName,
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**paramList)
{        
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering AES_DBO_BlockTransferQueueCmdHandler::adminOperationCallback\n"));
	(void)paramList;
	(void)oiHandle;
	(void)invocation;
	(void)p_objName;
	(void)operationId;
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::adminOperationCallback\n"));
	return;

}//End of adminOperationCallback

/*===================================================================
   ROUTINE: apply
=================================================================== */
void AES_DBO_BlockTransferQueueCmdHandler::apply(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering AES_DBO_BlockTransferQueueCmdHandler::apply\n"));
	(void)oiHandle;
	(void)ccbId;
	//do nothing
	if(prevState!=0)
        {
                delete prevState;
                prevState = 0;
        }

	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::apply\n"));
}//End of apply

/*===================================================================
   ROUTINE: handle_input
=================================================================== */
int AES_DBO_BlockTransferQueueCmdHandler::handle_input(	ACE_HANDLE fd)
{
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering BlockTransferQueueOIThread\n"));

	AES_GCC_TRACE_MESSAGE("IMM event notified ! Start dispatching ... ");

	errno = 0;
	if (reactor()->resume_handler(fd)) {
		int errno_save = errno;
		AES_GCC_TRACE_MESSAGE("Call 'reactor()->resume_handler' failed. Error: %d", errno_save);
	}

	if (acs_apgcc_objectimplementerinterface_V3::dispatch(ACS_APGCC_DISPATCH_ALL) == ACS_CC_SUCCESS)
		AES_GCC_TRACE_MESSAGE("IMM event successfully dispatched!");
	else
		AES_GCC_TRACE_MESSAGE("IMM event NOT dispatched! IMM error_code == %d", acs_apgcc_objectimplementerinterface_V3::getInternalLastError());

	return 0;
}//End of BlockTransferQueueOIThread

/*===================================================================
   ROUTINE: setupBlockTransferQueueOIThread
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferQueueCmdHandler::setupBlockTransferQueueOIThread(AES_DBO_BlockTransferQueueCmdHandler * aBlockTransferQueueImpl, 
		ACE_Reactor* threadManager_)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	if(!threadManager_)
	{
		AES_GCC_TRACE_MESSAGE("FATAL - Empty reactor!");
		return ACS_CC_FAILURE;
	}
	m_poThreadManager = threadManager_;
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering setupBlockTransferQueueOIThread\n"));
	int result = 0;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	if( aBlockTransferQueueImpl->setObjectImplementer() != ACS_CC_SUCCESS)
	{
		GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) setObjectImplementer failed\n"));
		GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::setupBlockTransferQueueOIThread\n"));
		return ACS_CC_FAILURE;
	}
	AES_GCC_TRACE_MESSAGE("Registering handler into the reactor!");
	if((result = m_poThreadManager->register_handler(getSelObj(), this, ACE_Event_Handler::READ_MASK)) < 0)
	{
		AES_GCC_TRACE_MESSAGE("Error on Register handler! - error %d", result);
	//TODO - REMOVE CLASS IMPEMENTER
	}

	AES_GCC_TRACE_MESSAGE("Setup completed! Block Transfer queue command correctly initialized! ");
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving setupBlockTransferQueueOIThread\n"));
	return rc;

}//End of setupBlockTransferQueueOIThread

/*===================================================================
	ROUTINE: shutdown
=================================================================== */
void AES_DBO_BlockTransferQueueCmdHandler::shutdown()
{
	AES_GCC_TRACE_MESSAGE("Entering shutdown");

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	int op_res = m_poThreadManager->remove_handler(getSelObj(), ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL);
	if(!op_res)
		AES_GCC_TRACE_MESSAGE("Object unregistered from reactor");
	else
		AES_GCC_TRACE_MESSAGE("Error trying to unregistered object from reactor");

	if( m_isClassImplAdded == true )
	{

		if( m_poOiHandler.removeClassImpl(this, theClassName.c_str()) == ACS_CC_FAILURE )
		{
			GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) Error occurred while removing class implementer\n"));
			int intErr = getInternalLastError();
			GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) Error occurred while removing class implementer, ErrCode = %d\n", intErr));
		}
		else
		{
			m_isClassImplAdded = false;
		}

	}
	AES_GCC_TRACE_MESSAGE("Leaving shutdown");
}//End of shutdown

/*===================================================================
	ROUTINE: ~AES_DBO_BlockTransferQueueCmdHandler
=================================================================== */
AES_DBO_BlockTransferQueueCmdHandler::~AES_DBO_BlockTransferQueueCmdHandler()
{
	AES_GCC_TRACE_MESSAGE("Destructor");

}//End of Destructor

/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void AES_DBO_BlockTransferQueueCmdHandler::setErrorCode(unsigned int code, const char *objName)
{
	string ertxt = AES_DBO_TQManager::instance()->getErrorText(code);

	if( code == AES_UNREASVAL_BLOCKDESTSET || code == AES_UNREASVAL_BLOCKTQNAME ||
			code == AES_UNREASVAL_BLOCKREMDELAY || code == AES_UNREASVAL_BLOCKMIRROR )
	{
		ertxt = ertxt + " in " + objName;
	}

	GCC_TDEBUG((AES_DBO_block_transferq_handler,"setExitCode: exitCode = %d , errorText = %s\n", code, ertxt.c_str()));
	setExitCode(code, ertxt);
}

/*===================================================================
    ROUTINE: extractTQName
=================================================================== */
void AES_DBO_BlockTransferQueueCmdHandler::extractTQName(const char * attrName,
        const string attrType,
        string &tqName)
{
    GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering extractTQName\n"));

    AES_GCC_TRACE_MESSAGE("attrName = %s!", attrName);
    char tqname_temp[AES_DBO_IMM_DN_PATH_SIZE_MAX] = {0};

  	::sscanf(attrName, AES_DBO_BLOCK_TRANSFER_QUEUE_RDN"=%[^,]", tqname_temp);
  	 tqName = tqname_temp;
  	AES_GCC_TRACE_MESSAGE("%s TQ name retrieved = %s!", attrType.c_str(), tqName.c_str());
}


bool AES_DBO_BlockTransferQueueCmdHandler::validTQName(string str)
{
    GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering AES_DBO_BlockTransferQueueCmdHandler::validTQName\n"));
	const string legalChars(
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	const string legalChars2(
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	if (str.find_first_not_of(legalChars) == string::npos)
	{
		if (str.substr(0,1).find_first_not_of(legalChars2) == string::npos)
		{
			if (str.length() < 33)
			{
				GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::validTQName with true\n"));
				return true;
			}
		}
	}
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::validTQName with false\n"));
	return false;
}

bool AES_DBO_BlockTransferQueueCmdHandler::validDestSetName(string str)
{
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering AES_DBO_BlockTransferQueueCmdHandler::validDestSetName\n"));

    const string legalChars(
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    const string legalChars2(
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    if (str.find_first_not_of(legalChars) == string::npos)
    {
        if (str.substr(0,1).find_first_not_of(legalChars2) == string::npos)
        {
            if (str.length() < 33)
            {
				GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::validDestSetName with true\n"));
                return true;
            }
        }
    }
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::validDestSetName with false\n"));
    return false;
}
ACS_CC_ReturnType AES_DBO_BlockTransferQueueCmdHandler::updateRefAttibutes(
								ACS_APGCC_CcbHandle ccbHandleVal,
								ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal,
								string objName,
								string attrName,
								string attrVal)

{
	AES_GCC_TRACE_MESSAGE("Updating Ref Attributes...");
	GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Entering AES_DBO_BlockTransferQueueCmdHandler::updateRefAttibutes\n"));

    	ACS_CC_ReturnType retVal = ACS_CC_FAILURE;
	//Create the value.
    	string parent = AES_GCC_Util::dnOfBlockTransferM;
	char myDest[AES_DBO_IMM_DN_PATH_SIZE_MAX] = {0};
	::snprintf (myDest, AES_DBO_IMM_DN_PATH_SIZE_MAX,"%s=%s,%s", AES_DBO_BLOCK_DESTINATION_SET_ID_ATTR, attrVal.c_str(),parent.c_str());

	ACS_CC_ImmParameter parToModify;
	void * attr_values [1] = {0};
	attr_values [0] = reinterpret_cast<void *>(const_cast<char*>(myDest));
	parToModify.attrValues = attr_values;
	parToModify.attrName = const_cast<char*>(attrName.c_str());
	parToModify.attrType = ATTR_NAMET;
	parToModify.attrValuesNum = 1;
	AES_GCC_TRACE_MESSAGE("Updating Attribute %s with value '%s'", attrName.c_str(), myDest);
    	// Modify the object
    	retVal = modifyObjectAugmentCcb(ccbHandleVal,
                    adminOwnwrHandlerVal,
                    objName.c_str(),
                    &parToModify);
    	if( retVal != ACS_CC_SUCCESS )
    	{
    		GCC_TERROR((AES_DBO_block_transferq_handler,"(%t) Error occurred while modifying %s : %s\n", parToModify.attrName, getInternalLastErrorText()));
    		GCC_TTRACE((AES_DBO_block_transferq_handler,"(%t) Leaving AES_DBO_BlockTransferQueueCmdHandler::updateRefAttibutes\n"));
    		return ACS_CC_FAILURE;
    	}
    	AES_GCC_TRACE_MESSAGE("Update successfully dispatched!");
    	GCC_TDEBUG((AES_DBO_block_transferq_handler,"(%t) AES_DBO_BlockTransferQueueCmdHandler::update ref success\n"));
    	return ACS_CC_SUCCESS;
}
