//	********************************************************
//
//	 COPYRIGHT Ericsson 2015
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2015.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2015 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	********************************************************
#include "imm/handler.h"
#include "imm/imm.h"

#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "engine/workingset.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_IMM_OI_Handler)

namespace imm
{

	OI_Handler::OI_Handler()
	: 	m_oiHandler(),
		m_blockPeerImplementer(imm::moc_name::BLOCK_PEER, imm::oi_name::BLOCK_PEER),
		m_dataSinkImplementer(imm::moc_name::DATA_SINK, imm::oi_name::DATA_SINK),
		m_dataSourceImplementer(),
		m_fileFormatImplementer(imm::moc_name::FILE_FORMAT, imm::oi_name::FILE_FORMAT),
		m_filePeerImplementer(imm::moc_name::FILE_PEER, imm::oi_name::FILE_PEER),
		m_outputFormatImplementer(imm::moc_name::OUTPUT_FORMAT, imm::oi_name::OUTPUT_FORMAT),
		m_transferProgressImplementer(imm::moc_name::TRANSFER_PROGRESS, imm::oi_name::TRANSFER_PROGRESS)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	OI_Handler::~OI_Handler()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int OI_Handler::registerInImm(Base_OI &oi)
	{
		if ((oi.isImmRegistered() == false) && (ACS_CC_FAILURE == m_oiHandler.addClassImpl(&oi, oi.getIMMClassName())))
		{
			//Error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Cannot register %s. See ACS_APGCC.log", oi.getIMMClassName());
			AES_DDT_TRACE_MESSAGE("ERROR: Cannot register %s. See ACS_APGCC.log", oi.getIMMClassName());
			return common::errorCode::ERR_GENERIC;
		}
		else
		{
			oi.setImmRegistration();

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Registered Object Implementer for <%s>", oi.getIMMClassName());
			AES_DDT_TRACE_MESSAGE("Registered Object Implementer for <%s>", oi.getIMMClassName());

			return common::errorCode::ERR_NO_ERRORS;
		}
	}

	int OI_Handler::registerInReactor(Base_OI &oi)
	{
		errno = 0;
		if ( (oi.isReatorRegistered() == false) && (engine::workingSet_t::instance()->getMainReactor().register_handler(oi.getSelObj(), &oi, ACE_Event_Handler::READ_MASK) < 0))
		{
			//Error
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'register_handler' failed: cannot register <%s> OI handler into the main reactor", oi.getIMMClassName());
			AES_DDT_TRACE_MESSAGE("ERROR: Call 'register_handler' failed: cannot register <%s> OI handler into the main reactor. errno <%d>", oi.getIMMClassName(), errno);
			return common::errorCode::ERR_REACTOR_HANDLER_REGISTER_FAILURE;
		}
		else
		{
			oi.setReactorRegistration();

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "OI <%s> registered into main reactor", oi.getIMMClassName());
			AES_DDT_TRACE_MESSAGE("OI <%s> registered into main reactor", oi.getIMMClassName());
			return common::errorCode::ERR_NO_ERRORS;
		}
	}

	int OI_Handler::take_ownership()
	{
		AES_DDT_TRACE_FUNCTION;
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "In");

		//--------------------------------------------------
		// Initialize IMM and set Implementers in the map
		AES_DDT_LOG(LOG_LEVEL_DEBUG, " ---- REGISTER IMPLEMENTERS ----");
		AES_DDT_TRACE_MESSAGE("---- REGISTER IMPLEMENTERS ----");

		int result = common::errorCode::ERR_NO_ERRORS;

		result = registerInImm(m_blockPeerImplementer);		if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInImm(m_dataSinkImplementer);		if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInImm(m_dataSourceImplementer);	if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInImm(m_fileFormatImplementer);	if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInImm(m_filePeerImplementer);		if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInImm(m_outputFormatImplementer);	if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInImm(m_transferProgressImplementer);	if(common::errorCode::ERR_NO_ERRORS != result) return result;

		//----------------------------------------------------------------------------------
		// register Implementers into the main reactor to receive IMM event notifications
		AES_DDT_LOG(LOG_LEVEL_DEBUG, " ---- REGISTER INTO REACTOR ----");
		AES_DDT_TRACE_MESSAGE("---- REGISTER INTO REACTOR ----");

		result = registerInReactor(m_blockPeerImplementer);		if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInReactor(m_dataSinkImplementer);		if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInReactor(m_dataSourceImplementer);	if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInReactor(m_fileFormatImplementer);	if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInReactor(m_filePeerImplementer);		if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInReactor(m_outputFormatImplementer);	if(common::errorCode::ERR_NO_ERRORS != result) return result;
		result = registerInReactor(m_transferProgressImplementer);	if(common::errorCode::ERR_NO_ERRORS != result) return result;

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Out");
		return common::errorCode::ERR_NO_ERRORS;
	}



	int OI_Handler::unregisterFromReactor(Base_OI &oi)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		if(oi.isReatorRegistered())
		{
			errno = 0;
			int reactorResult = engine::workingSet_t::instance()->getMainReactor().remove_handler(oi.getSelObj(), ACE_Event_Handler::READ_MASK);
			if(reactorResult < 0)
			{
				// ERROR: Removing the event handler from the reactor
				AES_DDT_LOG(LOG_LEVEL_ERROR,
							"Call 'remove_handler' failed: trying to remove the OI of MOC:<%s> from the main reactor. "
							"Error:<%d>",  oi.getIMMClassName(),  errno);

				result = common::errorCode::ERR_REACTOR_HANDLER_REMOVE_FAILURE;
			}
			else
			{
				oi.unsetReactorRegistration();
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "Handler of MOC:<%s> removed from the main reactor", oi.getIMMClassName());
			}
		}
		return result;
	}

	int OI_Handler::unregisterFromImm(Base_OI &oi)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		if( oi.isImmRegistered() && ( ACS_CC_FAILURE == m_oiHandler.removeClassImpl(&oi, oi.getIMMClassName()) ) )
		{
			//Error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "removeClassImpl failed for MOC:<%s>.", oi.getIMMClassName());
			result = common::errorCode::ERR_API_CALL;
		}
		else
		{
			oi.unsetImmRegistration();
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "removeClassImpl for MOC:<%s> DONE.", oi.getIMMClassName());
		}

		return result;
	}

	int OI_Handler::release_ownership()
	{
		AES_DDT_TRACE_FUNCTION;
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "In");

		//--------------------------------------------------------------------------
		// Remove OIs from main reactor
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Remove implementers from main reactor");
		AES_DDT_TRACE_MESSAGE("Remove implementers from main reactor");
		int transferProgressReactorResult =	unregisterFromReactor(m_transferProgressImplementer);
		int outputFormatReactorResult =	unregisterFromReactor(m_outputFormatImplementer);
		int filePeerReactorResult =	unregisterFromReactor(m_filePeerImplementer);
		int fileFormatReactorResult = unregisterFromReactor(m_fileFormatImplementer);
		int dataSourceReactorResult = unregisterFromReactor(m_dataSourceImplementer);
		int dataSinkReactorResult = unregisterFromReactor(m_dataSinkImplementer);
		int blockPeerReactorResult = unregisterFromReactor(m_blockPeerImplementer);


		if (transferProgressReactorResult || outputFormatReactorResult || filePeerReactorResult || fileFormatReactorResult || dataSourceReactorResult || dataSinkReactorResult || blockPeerReactorResult)
		{
			return common::errorCode::ERR_REACTOR_HANDLER_REMOVE_FAILURE;
		}
		else
		{
			//Success
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "All handlers have successfully been removed from the main reactor");
		}

		//------------------------------------------------------------------------
		// Remove OIs from IMM
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Remove implementers from IMM");
		AES_DDT_TRACE_MESSAGE("Remove implementers from IMM");
		int transferProgressImmResult =	unregisterFromImm(m_transferProgressImplementer);
		int outputFormatImmResult =	unregisterFromImm(m_outputFormatImplementer);
		int filePeerImmResult =	unregisterFromImm(m_filePeerImplementer);
		int fileFormatImmResult = unregisterFromImm(m_fileFormatImplementer);
		int dataSourceImmResult = unregisterFromImm(m_dataSourceImplementer);
		int dataSinkImmResult = unregisterFromImm(m_dataSinkImplementer);
		int blockPeerImmResult = unregisterFromImm(m_blockPeerImplementer);

		if (transferProgressImmResult || outputFormatImmResult || filePeerImmResult || fileFormatImmResult || dataSourceImmResult || dataSinkImmResult || blockPeerImmResult)
		{
			//Error
			return common::errorCode::ERR_CLOSE;
		}
		else
		{
			//Success
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "All handlers have successfully been removed from main reactor");
		}

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Out");
		return common::errorCode::ERR_NO_ERRORS;
	}


} /* namespace ddt_imm */
