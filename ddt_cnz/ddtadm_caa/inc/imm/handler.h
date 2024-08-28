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
#ifndef DDTADM_CAA_INC_AES_DDT_IMM_HANDLER_H_
#define DDTADM_CAA_INC_AES_DDT_IMM_HANDLER_H_

#ifndef CUTE_TEST
	#include "imm/blockpeer_oi.h"
	#include "imm/datasink_oi.h"
	#include "imm/datasource_oi.h"
	#include "imm/fileformat_oi.h"
	#include "imm/filepeer_oi.h"
	#include "imm/outputformat_oi.h"
	#include "imm/transferprogress_oi.h"
	#include <acs_apgcc_oihandler_V3.h>
	#include <boost/noncopyable.hpp>
#else
	#include "stubs/imm_stubs.h"
#endif

namespace imm
{
	/**
	 * @class OI_Handler
	 *
	 * @brief
	 * Manages both initialization and release of IMM related objects.
	 *
	 * @sa Base_OI
	 */
	class OI_Handler : private boost::noncopyable
	{
 	public:

		/// Constructor.
		OI_Handler();

		/// Destructor.
		virtual ~OI_Handler();

		/**
		 * @brief take_ownership performs the initialization of IMM objects
		 * @return int: 0 no errors
		 * @exception	none
		 */
		int take_ownership();

		/**
		 * @brief release_ownership releases IMM objects
		 * @return int: 0 no errors
		 * @exception	none
		 */
		int release_ownership();

 	private:

		/**	@brief
		 *
		 *	This method registers implementer to the given MOC in IMM.
		 *
		 *	@param oi: instance of Base_OI.
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		int registerInImm(Base_OI &oi);

		/**	@brief
		 *
		 *	This method registers reactor.
		 *
		 *	@param oi: instance of Base_OI.
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		int registerInReactor(Base_OI &oi);

		/**	@brief
		 *
		 *	This method unregisters implementer to the given MOC in IMM.
		 *
		 *	@param oi: instance of Base_OI.
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		int unregisterFromImm(Base_OI &oi);

		/**	@brief
		 *
		 *	This method unregisters reactor.
		 *
		 *	@param oi: instance of Base_OI.
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		int unregisterFromReactor(Base_OI &oi);

 	private:
		/* @brief m_oiHandler handles all Object Implementers */
		acs_apgcc_oihandler_V3 m_oiHandler;

		/* @brief m_blockPeerImplementer Object Implementer of BlockPeer MO */
		BlockPeer_OI	m_blockPeerImplementer;

		/* @brief m_dataSinkImplementer Object Implementer of DataSink MO */
		DataSink_OI		m_dataSinkImplementer;

		/* @brief m_dataSourceImplementer Object Implementer of DataSource MO */
		DataSource_OI	m_dataSourceImplementer;

		/* @brief m_fileFormatImplementer Object Implementer of FileFormat MO */
		FileFormat_OI	m_fileFormatImplementer;

		/* @brief m_filePeerImplementer Object Implementer of FilePeer MO */
		FilePeer_OI		m_filePeerImplementer;

		/* @brief m_outputFormatImplementer Object Implementer of OutputFormat MO */
		OutputFormat_OI	m_outputFormatImplementer;

		/* @brief m_transferProgressImplementer Object Implementer of TransferProgress MO */
		TransferProgress_OI m_transferProgressImplementer;
	};

} /* namespace ddt_imm */

#endif /* DDTADM_CAA_INC_AES_DDT_IMM_HANDLER_H_ */
