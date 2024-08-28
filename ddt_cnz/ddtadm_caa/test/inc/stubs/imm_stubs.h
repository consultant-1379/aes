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
#ifndef IMM_STUBS_H_
#define IMM_STUBS_H_
#include "operation/operation.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include "stubs/ACE_Task_Base_stub.h"

#if 0
namespace common
{
	namespace utility
	{
		inline bool getLastFieldValue(const std::string& stringToParser, std::string& value)
		{
			return true;
		};

		inline const char* boolToString(bool b)
		{
			return b ? "TRUE" : "FALSE";
		}
	}
}
#endif
namespace ACS_APGCC
{
	inline void toUpper(const std::string str){};
}

typedef int ACE_HANDLE;
const int ACE_INVALID_HANDLE = -1;

typedef enum {
	ACS_CC_SUCCESS = 0,
	ACS_CC_FAILURE = 1
}ACS_CC_ReturnType;

typedef enum {
	CONFIGURATION = 1,
	RUNTIME       = 2

}ACS_CC_ClassCategoryType;

typedef enum {
	ATTR_INT32T  = 1, /* Int32T  int */
	ATTR_UINT32T = 2, /* Uint32T unsigned int */
	ATTR_INT64T  = 3, /* Int64T  long long */
	ATTR_UINT64T = 4, /* Uint64T unsigned long long*/
	ATTR_TIMET   = 5, /* TimeT   long long*/
	ATTR_NAMET   = 6, /* NameT   array of char*/
	ATTR_FLOATT  = 7, /* FloatT  float*/
	ATTR_DOUBLET = 8, /* DoubleT double*/
	ATTR_STRINGT = 9, /* StringT array of char*/
	ATTR_ANYT    = 10 /* AnyT */

}ACS_CC_AttrValueType;

typedef int ACS_CC_AttrFlagsT;
typedef struct {
	char * attrName;
	ACS_CC_AttrValueType attrType;
	ACS_CC_AttrFlagsT attrFlag;
	void* attrDefaultVal;
}ACS_CC_AttrDefinitionType;

/*Added For Object creation*/
typedef struct {
	char * attrName;
	ACS_CC_AttrValueType attrType;
	unsigned int attrValuesNum;
	void** attrValues;
}ACS_CC_ValuesDefinitionType;
/*End Added For Object creation*/

typedef enum {
	ACS_APGCC_ONE 		= 1,
	ACS_APGCC_SUBLEVEL 	= 2,
	ACS_APGCC_SUBTREE 	= 3
}ACS_APGCC_ScopeT;

typedef enum {
	ACS_APGCC_DISPATCH_ONE 		= 1,
	ACS_APGCC_DISPATCH_ALL 		= 2,
	ACS_APGCC_DISPATCH_BLOCKING = 3
}ACS_APGCC_DispatchFlags;

typedef enum {
	OI_HANDLE_FOR_CREATE,
	OI_HANDLE_FOR_MODIFY,
	OI_HANDLE_FOR_DELETE,
	OI_HANDLE_FOR_ADMOP
}ACS_APGCC_OiHandle;


typedef int ACS_APGCC_CcbId;

typedef struct {
	char * attrName;
	ACS_CC_AttrValueType attrType;
	unsigned int attrValuesNum;
	void** attrValues;
}ACS_APGCC_AttrValues;

typedef enum {
	ACS_APGCC_ATTR_VALUES_ADD 		= 1,
	ACS_APGCC_ATTR_VALUES_DELETE 	= 2,
	ACS_APGCC_ATTR_VALUES_REPLACE 	= 3
}ACS_APGCC_AttrModificationTypeT;

typedef struct {
	ACS_APGCC_AttrModificationTypeT  modType;
	ACS_APGCC_AttrValues modAttr;
}ACS_APGCC_AttrModification;

/**Define the wrapper for SaImmContinuationIdT the base type is SaUint64T*/
typedef int ACS_APGCC_ContinuationIdType;

/*Define the wrapper for SaImmAdminOperationIdTthe base type is SaUint64T;*/
typedef int ACS_APGCC_AdminOperationIdType;

/*Define the wrapper for SaInvocationT the base type is SaUint64T;*/
typedef int ACS_APGCC_InvocationType;

/*Define the wrapper for SaImmAdminOperationParamsT*/
typedef struct {
	char* attrName;
	ACS_CC_AttrValueType attrType;
	void* attrValues;
}ACS_APGCC_AdminOperationParamType;

//typedef int ACE_Reactor_Mask;
inline void setExitCode(int p_exitCode, const std::string str){};
struct ACS_CC_ImmParameter;
namespace imm
{
// CLASS BASE_OI
class Base_OI
{
public:
	///  Constructor.
	static int m_dataSourceCountCreated;
	static int m_dataSourceCountModified;
	static int m_dataSinkCountCreated;
	static int m_dataSinkCountModified;

	Base_OI(const std::string &managedObjectClassName, const std::string &m_objectImplementerName)
	{
		m_dataSourceCountCreated = 0;
		m_dataSourceCountModified = 0;
		m_dataSinkCountCreated = 0;
		m_dataSinkCountModified = 0;
	}

	/// Destructor is virtual to enable proper cleanup.
	virtual ~Base_OI(){}

	virtual inline int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE){return 0;}

	virtual inline int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask){return 0;}
	virtual inline ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr){return ACS_CC_SUCCESS;};
	virtual inline ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){return ACS_CC_SUCCESS;};

	virtual inline ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods){return ACS_CC_SUCCESS;};

	virtual inline ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){return ACS_CC_SUCCESS;};
	virtual inline void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){};
	virtual inline void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){};

	virtual inline ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName){return ACS_CC_SUCCESS;};

	virtual inline void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList){}

	const char* getIMMClassName() const {return m_managedObjectClassName.c_str();};

	const char* getObjectImplementerName() const {return m_objectImplementerName.c_str();};

	inline bool isImmRegistered() const { return m_immRegistered; }
	inline void setImmRegistration() { m_immRegistered = true; }
	inline void unsetImmRegistration() { m_immRegistered = false; }

	inline bool isReatorRegistered() const { return m_reactorRegistered; }
	inline void setReactorRegistration() { m_reactorRegistered = true; }
	inline void unsetReactorRegistration() { m_reactorRegistered = false; }

	ACS_CC_ReturnType modifyRuntimeObj( const char* objName,ACS_CC_ImmParameter * runtumeAttr)
	{
		if(strstr(objName,"dummy")) return ACS_CC_FAILURE;
		return ACS_CC_SUCCESS;
	}

	inline int getInternalLastError() { return 0; }
	inline const char* getInternalLastErrorText() { std::string errMsg("No Error"); return errMsg.c_str(); }


protected:

	enum ImmAction
	{
		Nothing = 0,
		Create,
		Delete,
		Modify
	};

	bool sendRequestAndWaitResult(const operation::identifier_t& id, const void* op_info, bool reportErrorToImm )
	{
		switch(id)
		{
		case operation::DATASOURCE_ADD: ++m_dataSourceCountCreated; break;
		case operation::DATASOURCE_MOD:
		{
			if(reportErrorToImm)
				++m_dataSourceCountModified;
			else --m_dataSourceCountModified;
		}
		break;

		case operation::DATASOURCE_DEL: --m_dataSourceCountCreated; break;

		case operation::DATASINK_ADD: ++m_dataSinkCountCreated; break;
		case operation::DATASINK_MOD:
		{
			if(reportErrorToImm)
				++m_dataSinkCountModified;
			else --m_dataSinkCountModified;
		}
		break;

		case operation::DATASINK_DEL: --m_dataSinkCountCreated; break;
		default: break;
		}
		std::cout<<"m_dataSourceCountCreated : "<<m_dataSourceCountCreated<<std::endl;
		std::cout<<"m_dataSourceCountModified : "<<m_dataSourceCountModified<<std::endl;

		std::cout<<"m_dataSinkCountCreated : "<<m_dataSinkCountCreated<<std::endl;
		std::cout<<"m_dataSinkCountModified : "<<m_dataSinkCountModified<<std::endl;
		return true;
	}

private:
	std::string m_managedObjectClassName;
	std::string m_objectImplementerName;

	bool m_immRegistered;
	bool m_reactorRegistered;

};

}
class ACS_APGCC_ImmAttribute {
public:
	std::string attrName;
	int attrType;
	unsigned int attrValuesNum;
	void* attrValues[2048];
	int flag;
	//        std::vector<Types *> pointers;
	ACS_APGCC_ImmAttribute(){;}
	~ACS_APGCC_ImmAttribute(){;}
};

/*Added For Attribute value Search*/
struct ACS_CC_ImmParameter {
	char * attrName;
	ACS_CC_AttrValueType attrType;
	unsigned int attrValuesNum;
	void** attrValues;
	int flag;
	ACS_CC_ImmParameter(){ flag = 0; }
	void ACS_APGCC_IMMFreeMemory(int f){ flag = f; }
	~ACS_CC_ImmParameter(){ if (flag != 0 ) delete [] attrValues; }
};

class OmHandler {
public:
	OmHandler(){}
	OmHandler(OmHandler &objImp){}
	virtual ~OmHandler(){}
	ACS_CC_ReturnType Init () {return ACS_CC_SUCCESS;}
	ACS_CC_ReturnType Finalize () {return ACS_CC_SUCCESS;}
	inline ACS_CC_ReturnType getAttribute (const char* p_objectName, std::vector<ACS_APGCC_ImmAttribute *> p_attributeList ){return ACS_CC_SUCCESS;}
	inline ACS_CC_ReturnType getChildren(const char* p_rootName, ACS_APGCC_ScopeT p_scope, std::vector<std::string>* p_rdnList){return ACS_CC_SUCCESS;}
	inline ACS_CC_ReturnType getLastErrorText(std::string p_parentDN, int& p_errorId, std::string& p_errorText){return ACS_CC_SUCCESS;}
	inline ACS_CC_ReturnType getExitCode( int& p_errorId, std::string& p_errorText){return ACS_CC_SUCCESS;}
	inline ACS_CC_ReturnType createObject (const char* m_className, const char* m_parentName, std::vector<ACS_CC_ValuesDefinitionType> m_attrValuesList ) {return ACS_CC_SUCCESS;}
	inline int getInternalLastError() { return 0; }
	inline const char* getInternalLastErrorText() { std::string errMsg("No Error"); return errMsg.c_str(); }

private:
	OmHandler& operator=(OmHandler &omHandler);
};

#endif /* IMM_STUBS_H_ */
