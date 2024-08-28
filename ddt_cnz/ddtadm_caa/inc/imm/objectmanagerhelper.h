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

#ifndef DDTADM_CAA_INC_IMM_OBJECTMANAGERHELPER_H_
#define DDTADM_CAA_INC_IMM_OBJECTMANAGERHELPER_H_

#include "imm/imm.h"
#include <boost/scoped_ptr.hpp>

#include <ace/Singleton.h>
#include <ace/Thread_Mutex.h>
#include "common/programconstants.h"

#include <string>
#include <vector>

class OmHandler;

namespace imm
{
	class ObjectManagerHelper
	{
		friend class ACE_Singleton<ObjectManagerHelper, ACE_Recursive_Thread_Mutex>;

	 public:

		/** @brief
		 *
		 *	This method changes the value of status attribute of a specific Peer.
		 *
		 *  @param peerDN: DN of peer
		 *
		 *  @param newStatus: new status value
		 *
		 *  @return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool changePeerStatusAttribute(const std::string& peerDN, int32_t newStatus);

		/** @brief
		 *
		 *	This method resets the attributes of a specific progressReport MO.
		 *
		 *  @param dataSinkDN: DN of dataSink
		 *
		 *	@remarks Remarks
		 */
		void initProgressReportForSwitchActivePeer(const std::string& dataSinkDN);

		/** @brief
		 *
		 *	This method resets the attributes of a specific progressReport MO.
		 *
		 *  @param dataSinkDN: DN of dataSink
		 *
		 *	@remarks Remarks
		 */
		void finalizeProgressReportForSwitchActivePeer(const std::string& dataSinkDN);

		/** @brief
		 *
		 *	This method updates the action name attribute of a specific progressReport MO.
		 *
		 *  @param dataSinkDN: DN of dataSink
		 *
		 *  @param name: Action name.
		 *
		 *	@remarks Remarks
		 */
		void updateActionName(const std::string& dataSinkDN, const std::string& name);

		/** @brief
		 *
		 *	This method updates the action id attribute of a specific progressReport MO.
		 *
		 *  @param dataSinkDN: DN of dataSink
		 *
		 *  @param name: Action id.
		 *
		 *	@remarks Remarks
		 */
		void updateActionId(const std::string& dataSinkDN, uint32_t id);

		/** @brief
		 *
		 *	This method updates the action result attribute of a specific progressReport MO.
		 *
		 *  @param dataSinkDN: DN of dataSink
		 *
		 *  @param result: Action result value.
		 *
		 *	@remarks Remarks
		 */
		void updateActionResult(const std::string& dataSinkDN, actionprogress::ResultType result);

		/** @brief
		 *
		 *	This method updates the resultInfo attribute of a specific progressReport MO.
		 *
		 *  @param dataSinkDN: DN of dataSink
		 *
		 *  @param result: Action result info.
		 *
		 *	@remarks Remarks
		 */
		void updateActionResultInfo(const std::string& dataSinkDN, const std::string& info);

		/** @brief
		 *
		 *	This method updates the action state attribute of a specific progressReport MO.
		 *
		 *  @param dataSinkDN: DN of dataSink
		 *
		 *  @param state: Action state.
		 *
		 *	@remarks Remarks
		 */
		void updateActionState(const std::string& dataSinkDN, actionprogress::StateType state);

		/** @brief
		 *
		 *	This method updates the action progressPercentage attribute of a specific progressReport MO.
		 *
		 *  @param dataSinkDN: DN of dataSink
		 *
		 *  @param progressPercentage: Action execution progress percentage.
		 *
		 *	@remarks Remarks
		 */
		void updateActionProgressPercentage(const std::string& dataSinkDN, uint32_t progressPercentage);

		/**	@brief
		 *
		 *  This method updates the action timeActionStarted attribute of a specific progressReport MO.
		 *
		 *	@param dataSinkDN: DN of dataSink
		 *
		 *	@remarks Remarks
		 */
		void updateTimeActionStarted(const std::string& dataSinkDN);

		/**	@brief
		 *
		 *  This method updates the timeActionCompleted attribute of a specific progressReport MO.
		 *
		 *  @param dataSinkDN: DN of dataSink
		 *
		 *	@remarks Remarks
		 */
		void updateTimeActionCompleted(const std::string& dataSinkDN);

		/**	@brief
		 *
		 *  This method resets the timeActionCompleted attribute of a specific progressReport MO.
		 *
		 *	@param dataSinkDN: DN of dataSink
		 *
		 *	@remarks Remarks
		 */
		void resetTimeActionCompleted(const std::string& dataSinkDN);

		/**	@brief
		 *
		 * 	This method updates the timeOfLastStatusUpdate attribute of a specific progressReport MO.
		 *
		 * 	@param dataSinkDN: DN of dataSink
		 *
		 *	@remarks Remarks
		 */
		void updateTimeOfLastStatusUpdate(const std::string& dataSinkDN);

		/** @brief	getManagedElementId
		 *
		 *  This method gets the managed element Id.
		 *
		 *  @return managed element Id as a string.
		 *
		 *  @remarks Remarks
		 */
		std::string getManagedElementId();

		/** @brief	getACAMessageStoreNames
		 *
		 *  This method gets the MessageStores names defined in ACA Service
		 *
		 *  @param Vector returns ACA MessageStore Names
		 *
		 *  @return error code
		 *
		 *  @remarks Remarks
		 */
		common::errorCode::ErrorConstants getACAMessageStoreNames(std::vector<std::string> & msNames);

		/** @brief	getPassword
		 *
		 *  This method gets the password in cleartext from the specified ECIMPassword structure
		 *
		 *  @param passwordDN: the distinguished name of ECIMPassword structure
		 *
		 *  @param password: returns the password in cleartext
		 *
		 *  @return error code
		 *
		 *  @remarks Remarks
		 */
		common::errorCode::ErrorConstants getPassword(const std::string& passwordDN, std::string& password);

	 private:

		///  Constructor.
		ObjectManagerHelper();

		///  Destructor.
		virtual ~ObjectManagerHelper();

		/// initialize the object manager
		bool initObjManager();

		/**
		 * 	@brief  Update an attribute of progressReport structure
		 */
		void updateReportAttribute(const std::string& dataSinkDN, char* attributeName, const std::string& attributeValue);

		/**
		 * 	@brief  Update an attribute of progressReport structure
		 */
		void updateReportAttribute(const std::string& dataSinkDN, char* attributeName, int32_t attributeValue);

		/**
		 * 	@brief  Update an attribute of progressReport structure
		 */
		void updateReportAttribute(const std::string& dataSinkDN, char* attributeName, uint32_t attributeValue);

		/**
		 * 	@brief  Get date and time string
		 */
		void getDateAndTime(std::string& currentDateAndTime);

		// IMM object manager
		boost::scoped_ptr<OmHandler> m_objectManager;

		// Initialize flag
		bool m_ObjManagerInitialized;

		// mutual access to the m_objectManager
		ACE_Recursive_Thread_Mutex m_mutex;

	};

	typedef ACE_Singleton<ObjectManagerHelper, ACE_Recursive_Thread_Mutex> objectManagerHelper_t;

} /* namespace imm */

#endif /* DDTADM_CAA_INC_IMM_OBJECTMANAGERHELPER_H_ */
