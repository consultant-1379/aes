#include "cute.h"

#include "imm/DataSinkOITest.h"
#include "stubs/imm_stubs.h"
#include "imm/imm.h"
#include <iostream>
#include <string>
int imm::Base_OI::m_dataSinkCountCreated=0;
int imm::Base_OI::m_dataSinkCountModified=0;

imm::DataSink_OI DataSinkOITest::dataSinkImpl(imm::moc_name::DATA_SINK, imm::oi_name::DATA_SINK);

//Adding one data sink successfully
void DataSinkOITest::dataSinkAddSuccess() {
	ACS_APGCC_OiHandle oiHandle = OI_HANDLE_FOR_CREATE;
	ACS_APGCC_CcbId ccbId = (ACS_APGCC_CcbId)001;
	std::string className = imm::moc_name::DATA_SINK;
	std::string parentname = imm::mom_root::DDTM;

	int numOfAttributes = 3;
	ACS_APGCC_AttrValues * attr[numOfAttributes + 1];
	ACS_APGCC_AttrValues attrList[numOfAttributes];

	for (int i = 0 ; i< numOfAttributes ; i++)
	{
		attr[i] = &attrList[i];
	}
	attr[numOfAttributes] = NULL;

	std::string tempstr("dataSinkId=SINK1");
	std::string attrName1 = imm::datasink_attribute::RDN;
	size_t len = attrName1.size();
	attr[0]->attrName = new char[len+1];
	memset(attr[0]->attrName,0,len+1);
	strncpy(attr[0]->attrName,attrName1.c_str(),len+1);
	attr[0]->attrType = ATTR_STRINGT;
	attr[0]->attrValuesNum =1;
	void * tempVal[1] = {reinterpret_cast<void*>(const_cast<char*>(tempstr.c_str()))};
	attr[0]->attrValues = tempVal;

	//RETRYATTEMPTS
	std::string attrName2 = imm::datasink_attribute::RETRYATTEMPTS;
	len = attrName2.size();
	int retryAttempts = 10;
	attr[1]->attrName = new char[len+1];
	memset(attr[1]->attrName,0,len+1);
	strncpy(attr[1]->attrName,attrName2.c_str(),len+1);
	attr[1]->attrType = ATTR_UINT32T;
	attr[1]->attrValuesNum =1;
	void * tempVal1[1] = {reinterpret_cast<void*>(&retryAttempts)};
	attr[1]->attrValues = tempVal1;

	//RETRYDELAY
	std::string attrName3 = imm::datasink_attribute::RETRYDELAY;
	int retryDelay = 1;
	len = attrName3.size();
	attr[2]->attrName = new char[len+1];
	memset(attr[2]->attrName,0,len+1);
	::strncpy(attr[2]->attrName,attrName3.c_str(),len+1);
	attr[2]->attrType = ATTR_UINT32T;
	attr[2]->attrValuesNum =1;
	void * tempVal2[1] = {reinterpret_cast<void*>(&retryDelay)};
	attr[2]->attrValues = tempVal2;


	dataSinkImpl.create(oiHandle,ccbId,className.c_str(),parentname.c_str(),attr);
	ACS_CC_ReturnType retType=dataSinkImpl.complete(oiHandle,ccbId);
	dataSinkImpl.apply(oiHandle,ccbId);
	ASSERT_EQUAL(ACS_CC_SUCCESS , retType);

	//std::cout<<"Base_OI::m_dataSinkCountCreated : "<<imm::Base_OI::m_dataSinkCountCreated<<std::endl;
	//ASSERT_EQUAL(1 , imm::Base_OI::m_dataSinkCountCreated);

}
// Addition of another data sink will fail so,datasource count remain same.
void DataSinkOITest::dataSinkAddAbort() {
	ACS_APGCC_OiHandle oiHandle = OI_HANDLE_FOR_CREATE;
	ACS_APGCC_CcbId ccbId = (ACS_APGCC_CcbId)002;
	std::string className = imm::moc_name::DATA_SINK;
	std::string parentname = imm::mom_root::DDTM;
	int numOfAttributes = 3;
	ACS_APGCC_AttrValues * attr[numOfAttributes + 1];
	ACS_APGCC_AttrValues attrList[numOfAttributes];

	for (int i = 0 ; i< numOfAttributes ; i++)
	{
		attr[i] = &attrList[i];
	}
	attr[numOfAttributes] = NULL;

	std::string tempstr("dataSinkId=SINK1");
	std::string attrName1 = imm::datasink_attribute::RDN;
	size_t len = attrName1.size();
	attr[0]->attrName = new char[len+1];
	memset(attr[0]->attrName,0,len+1);
	strncpy(attr[0]->attrName,attrName1.c_str(),len+1);
	attr[0]->attrType = ATTR_STRINGT;
	attr[0]->attrValuesNum =1;
	void * tempVal[1] = {reinterpret_cast<void*>(const_cast<char*>(tempstr.c_str()))};
	attr[0]->attrValues = tempVal;

	//RETRYATTEMPTS
	std::string attrName2 = imm::datasink_attribute::RETRYATTEMPTS;
	len = attrName2.size();
	int retryAttempts = 10;
	attr[1]->attrName = new char[len+1];
	memset(attr[1]->attrName,0,len+1);
	strncpy(attr[1]->attrName,attrName2.c_str(),len+1);
	attr[1]->attrType = ATTR_UINT32T;
	attr[1]->attrValuesNum =1;
	void * tempVal1[1] = {reinterpret_cast<void*>(&retryAttempts)};
	attr[1]->attrValues = tempVal1;

	//RETRYDELAY
	std::string attrName3 = imm::datasink_attribute::RETRYDELAY;
	int retryDelay = 1;
	len = attrName3.size();
	attr[2]->attrName = new char[len+1];
	memset(attr[2]->attrName,0,len+1);
	::strncpy(attr[2]->attrName,attrName3.c_str(),len+1);
	attr[2]->attrType = ATTR_UINT32T;
	attr[2]->attrValuesNum =1;
	void * tempVal2[1] = {reinterpret_cast<void*>(&retryDelay)};
	attr[2]->attrValues = tempVal2;

	dataSinkImpl.create(oiHandle,ccbId,className.c_str(),parentname.c_str(),attr);
	ACS_CC_ReturnType retType=dataSinkImpl.complete(oiHandle,ccbId);
	dataSinkImpl.abort(oiHandle,ccbId);
	ASSERT_EQUAL(ACS_CC_SUCCESS , retType);
	//ASSERT_EQUAL(1 ,imm::Base_OI::m_dataSinkCountCreated);
}
/*void DataSinkOITest::dataSinkModifySuccess() {
	ACS_APGCC_OiHandle oiHandle = OI_HANDLE_FOR_MODIFY;;
	ACS_APGCC_CcbId ccbId = (ACS_APGCC_CcbId)003;
	std::string objName = "dataSourceId=VCHS1";
	int numOfAttributes = 2;
	ACS_APGCC_AttrModification * attrMod[numOfAttributes+1];
	ACS_APGCC_AttrModification attrModList[numOfAttributes];

	//Modification of ALARMLEVEL
	std::string attrName2 = imm::datasource_attribute::ALARMLEVEL;
	int len = attrName2.size();
	int alamLevel = 1;
	attrModList[0].modType = ACS_APGCC_ATTR_VALUES_REPLACE;
	attrModList[0].modAttr.attrName = new char[len+1];
	memset(attrModList[0].modAttr.attrName,0,len+1);
	strncpy(attrModList[0].modAttr.attrName,attrName2.c_str(),len+1);
	attrModList[0].modAttr.attrType = ATTR_UINT32T;
	attrModList[0].modAttr.attrValuesNum =1;
	void * tempVal1[1] = {reinterpret_cast<void*>(&alamLevel)};
	attrModList[0].modAttr.attrValues = tempVal1;

	//Modification of RECORDSIZE
	attrModList[1].modType = ACS_APGCC_ATTR_VALUES_REPLACE;
	std::string attrName3 = imm::datasource_attribute::RECORDSIZE;
	int recordSize = 2048;
	len = attrName3.size();
	attrModList[1].modAttr.attrName = new char[len+1];
	memset(attrModList[1].modAttr.attrName,0,len+1);
	::strncpy(attrModList[1].modAttr.attrName,attrName3.c_str(),len+1);
	attrModList[1].modAttr.attrType = ATTR_UINT32T;
	attrModList[1].modAttr.attrValuesNum =1;
	void * tempVal2[1] = {reinterpret_cast<void*>(&recordSize)};
	attrModList[1].modAttr.attrValues = tempVal2;

	for (int i = 0 ; i< numOfAttributes ; i++)
	{
		attrMod[i] = &attrModList[i];
	}
	attrMod[numOfAttributes] = NULL;

	dataSinkImpl.modify(oiHandle,ccbId,objName.c_str(),attrMod);
	dataSinkImpl.complete(oiHandle,ccbId);
	dataSinkImpl.apply(oiHandle,ccbId);
	ASSERT_EQUAL(1 ,imm::Base_OI::m_dataSourceCountModified);
}
void DataSourceOITest::dataSourceModifyAbort() {

	ACS_APGCC_OiHandle oiHandle = OI_HANDLE_FOR_MODIFY;;
	ACS_APGCC_CcbId ccbId = (ACS_APGCC_CcbId)004;
	std::string objName = "dataSourceId=VCHS1";
	int numOfAttributes = 2;
	ACS_APGCC_AttrModification * attrMod[numOfAttributes+1];
	ACS_APGCC_AttrModification attrModList[numOfAttributes];

	//Modification of ALARMLEVEL
	std::string attrName2 = imm::datasource_attribute::ALARMLEVEL;
	int len = attrName2.size();
	int alamLevel = 2;
	attrModList[0].modType = ACS_APGCC_ATTR_VALUES_REPLACE;
	attrModList[0].modAttr.attrName = new char[len+1];
	memset(attrModList[0].modAttr.attrName,0,len+1);
	strncpy(attrModList[0].modAttr.attrName,attrName2.c_str(),len+1);
	attrModList[0].modAttr.attrType = ATTR_UINT32T;
	attrModList[0].modAttr.attrValuesNum =1;
	void * tempVal1[1] = {reinterpret_cast<void*>(&alamLevel)};
	attrModList[0].modAttr.attrValues = tempVal1;

	//Modification of RECORDSIZE
	attrModList[1].modType = ACS_APGCC_ATTR_VALUES_REPLACE;
	std::string attrName3 = imm::datasource_attribute::RECORDSIZE;
	int recordSize = 4096;
	len = attrName3.size();
	attrModList[1].modAttr.attrName = new char[len+1];
	memset(attrModList[1].modAttr.attrName,0,len+1);
	::strncpy(attrModList[1].modAttr.attrName,attrName3.c_str(),len+1);
	attrModList[1].modAttr.attrType = ATTR_UINT32T;
	attrModList[1].modAttr.attrValuesNum =1;
	void * tempVal2[1] = {reinterpret_cast<void*>(&recordSize)};
	attrModList[1].modAttr.attrValues = tempVal2;

	for (int i = 0 ; i< numOfAttributes ; i++)
	{
		attrMod[i] = &attrModList[i];

	}
	attrMod[numOfAttributes] = NULL;

	dataSourceImpl.modify(oiHandle,ccbId,objName.c_str(),attrMod);
	dataSourceImpl.complete(oiHandle,ccbId);
	dataSourceImpl.abort(oiHandle,ccbId);
	ASSERT_EQUAL(2 ,imm::Base_OI::m_dataSourceCountModified);
} */
//There is an existing object, below test case failed to remove
void DataSinkOITest::dataSinkDeleteAbort() {

	std::string objName("dataSinkId=SINK1");
	ACS_APGCC_OiHandle oiHandle = OI_HANDLE_FOR_DELETE;
	ACS_APGCC_CcbId ccbId = (ACS_APGCC_CcbId)005;

	dataSinkImpl.deleted(oiHandle,ccbId,objName.c_str());
	ACS_CC_ReturnType retType=dataSinkImpl.complete(oiHandle,ccbId);
	dataSinkImpl.abort(oiHandle,ccbId);

	ASSERT_EQUAL(ACS_CC_SUCCESS , retType);

	//ASSERT_EQUAL(1 ,imm::Base_OI::m_dataSinkCountCreated);
}
void DataSinkOITest::dataSinkDeleteSuccess() {
	std::string objName("dataSinkId=SINK1");
	ACS_APGCC_OiHandle oiHandle = OI_HANDLE_FOR_DELETE;
	ACS_APGCC_CcbId ccbId = (ACS_APGCC_CcbId)006;

	dataSinkImpl.deleted(oiHandle,ccbId,objName.c_str());
	ACS_CC_ReturnType retType=dataSinkImpl.complete(oiHandle,ccbId);
	dataSinkImpl.apply(oiHandle,ccbId);
	ASSERT_EQUAL(ACS_CC_SUCCESS , retType);
	ASSERT_EQUAL(0 ,imm::Base_OI::m_dataSinkCountCreated);
}

cute::suite DataSinkOITest::make_suite_dataSinkOITest(){
	cute::suite s;
	s.push_back(CUTE(DataSinkOITest::dataSinkAddSuccess));
	s.push_back(CUTE(DataSinkOITest::dataSinkAddAbort));
	//s.push_back(CUTE(DataSourceOITest::dataSourceModifySuccess));
	//s.push_back(CUTE(DataSourceOITest::dataSourceModifyAbort));
	s.push_back(CUTE(DataSinkOITest::dataSinkDeleteAbort));
	s.push_back(CUTE(DataSinkOITest::dataSinkDeleteSuccess));
	return s;
}
