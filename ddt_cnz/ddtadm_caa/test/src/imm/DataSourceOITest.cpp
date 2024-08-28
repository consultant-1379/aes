#include "cute.h"

#include "imm/DataSourceOITest.h"
#include "stubs/imm_stubs.h"
#include "imm/imm.h"
#include <iostream>
#include <string>
int imm::Base_OI::m_dataSourceCountCreated=0;
int imm::Base_OI::m_dataSourceCountModified=0;

imm::DataSource_OI DataSourceOITest::dataSourceImpl;

//Adding one data source successfully
void DataSourceOITest::dataSourceAddSuccess() {
	ACS_APGCC_OiHandle oiHandle = OI_HANDLE_FOR_CREATE;
	ACS_APGCC_CcbId ccbId = (ACS_APGCC_CcbId)001;
	std::string className = imm::moc_name::DATA_SOURCE;;
	std::string parentname = imm::mom_root::DDTM;

	int numOfAttributes = 3;
	ACS_APGCC_AttrValues * attr[numOfAttributes + 1];
	ACS_APGCC_AttrValues attrList[numOfAttributes];

	for (int i = 0 ; i< numOfAttributes ; i++)
	{
		attr[i] = &attrList[i];
	}
	attr[numOfAttributes] = NULL;

	std::string tempstr("dataSourceId=VCHS1");
	std::string attrName1 = imm::datasource_attribute::RDN;
	size_t len = attrName1.size();
	attr[0]->attrName = new char[len+1];
	memset(attr[0]->attrName,0,len+1);
	strncpy(attr[0]->attrName,attrName1.c_str(),len+1);
	attr[0]->attrType = ATTR_STRINGT;
	attr[0]->attrValuesNum =1;
	void * tempVal[1] = {reinterpret_cast<void*>(const_cast<char*>(tempstr.c_str()))};
	attr[0]->attrValues = tempVal;

	//ALARMLEVEL
	std::string attrName2 = imm::datasource_attribute::ALARMLEVEL;
	len = attrName2.size();
	int alamLevel = 1;
	attr[1]->attrName = new char[len+1];
	memset(attr[1]->attrName,0,len+1);
	strncpy(attr[1]->attrName,attrName2.c_str(),len+1);
	attr[1]->attrType = ATTR_UINT32T;
	attr[1]->attrValuesNum =1;
	void * tempVal1[1] = {reinterpret_cast<void*>(&alamLevel)};
	attr[1]->attrValues = tempVal1;

	//RECORDSIZE
	std::string attrName3 = imm::datasource_attribute::RECORDSIZE;
	int recordSize = 2048;
	len = attrName3.size();
	attr[2]->attrName = new char[len+1];
	memset(attr[2]->attrName,0,len+1);
	::strncpy(attr[2]->attrName,attrName3.c_str(),len+1);
	attr[2]->attrType = ATTR_UINT32T;
	attr[2]->attrValuesNum =1;
	void * tempVal2[1] = {reinterpret_cast<void*>(&recordSize)};
	attr[2]->attrValues = tempVal2;


	dataSourceImpl.create(oiHandle,ccbId,className.c_str(),parentname.c_str(),attr);
	ACS_CC_ReturnType retType=dataSourceImpl.complete(oiHandle,ccbId);
	dataSourceImpl.apply(oiHandle,ccbId);
	ASSERT_EQUAL(ACS_CC_SUCCESS , retType);
	//std::cout<<"Base_OI::m_dataSourceCountCreated : "<<imm::Base_OI::m_dataSourceCountCreated<<std::endl;
	//ASSERT_EQUAL(1 , imm::Base_OI::m_dataSourceCountCreated);
}
// Addition of another data source will fail so,datasource count remain same.
void DataSourceOITest::dataSourceAddAbort() {
	ACS_APGCC_OiHandle oiHandle = OI_HANDLE_FOR_CREATE;
	ACS_APGCC_CcbId ccbId = (ACS_APGCC_CcbId)002;
	std::string className = imm::moc_name::DATA_SOURCE;;
	std::string parentname = imm::mom_root::DDTM;
	int numOfAttributes = 3;
	ACS_APGCC_AttrValues * attr[numOfAttributes + 1];
	ACS_APGCC_AttrValues attrList[numOfAttributes];

	for (int i = 0 ; i< numOfAttributes ; i++)
	{
		attr[i] = &attrList[i];
	}
	attr[numOfAttributes] = NULL;

	std::string tempstr("dataSourceId=VCHS1");
	std::string attrName1 = imm::datasource_attribute::RDN;
	size_t len = attrName1.size();
	attr[0]->attrName = new char[len+1];
	memset(attr[0]->attrName,0,len+1);
	strncpy(attr[0]->attrName,attrName1.c_str(),len+1);
	attr[0]->attrType = ATTR_STRINGT;
	attr[0]->attrValuesNum =1;
	void * tempVal[1] = {reinterpret_cast<void*>(const_cast<char*>(tempstr.c_str()))};
	attr[0]->attrValues = tempVal;

	//ALARMLEVEL
	std::string attrName2 = imm::datasource_attribute::ALARMLEVEL;
	len = attrName2.size();
	int alamLevel = 1;
	attr[1]->attrName = new char[len+1];
	memset(attr[1]->attrName,0,len+1);
	strncpy(attr[1]->attrName,attrName2.c_str(),len+1);
	attr[1]->attrType = ATTR_UINT32T;
	attr[1]->attrValuesNum =1;
	void * tempVal1[1] = {reinterpret_cast<void*>(&alamLevel)};
	attr[1]->attrValues = tempVal1;

	//RECORDSIZE
	std::string attrName3 = imm::datasource_attribute::RECORDSIZE;
	int recordSize = 2048;
	len = attrName3.size();
	attr[2]->attrName = new char[len+1];
	memset(attr[2]->attrName,0,len+1);
	::strncpy(attr[2]->attrName,attrName3.c_str(),len+1);
	attr[2]->attrType = ATTR_UINT32T;
	attr[2]->attrValuesNum =1;
	void * tempVal2[1] = {reinterpret_cast<void*>(&recordSize)};
	attr[2]->attrValues = tempVal2;

	dataSourceImpl.create(oiHandle,ccbId,className.c_str(),parentname.c_str(),attr);
	ACS_CC_ReturnType retType=dataSourceImpl.complete(oiHandle,ccbId);
	dataSourceImpl.abort(oiHandle,ccbId);
	ASSERT_EQUAL(ACS_CC_SUCCESS , retType);
	//ASSERT_EQUAL(1 ,imm::Base_OI::m_dataSourceCountCreated);
}
void DataSourceOITest::dataSourceModifySuccess() {
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

	dataSourceImpl.modify(oiHandle,ccbId,objName.c_str(),attrMod);
	ACS_CC_ReturnType retType=dataSourceImpl.complete(oiHandle,ccbId);
	dataSourceImpl.apply(oiHandle,ccbId);
	ASSERT_EQUAL(ACS_CC_SUCCESS , retType);
	//ASSERT_EQUAL(1 ,imm::Base_OI::m_dataSourceCountModified);
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
	ACS_CC_ReturnType retType=dataSourceImpl.complete(oiHandle,ccbId);
	dataSourceImpl.abort(oiHandle,ccbId);

	ASSERT_EQUAL(ACS_CC_SUCCESS , retType);
	//ASSERT_EQUAL(2 ,imm::Base_OI::m_dataSourceCountModified);
}
//There is an existing object, below test case failed to remove
void DataSourceOITest::dataSourceDeleteAbort() {

	std::string objName("dataSourceId=VCHS1");
	ACS_APGCC_OiHandle oiHandle = OI_HANDLE_FOR_DELETE;
	ACS_APGCC_CcbId ccbId = (ACS_APGCC_CcbId)005;

	dataSourceImpl.deleted(oiHandle,ccbId,objName.c_str());
	ACS_CC_ReturnType retType=dataSourceImpl.complete(oiHandle,ccbId);
	dataSourceImpl.abort(oiHandle,ccbId);
	ASSERT_EQUAL(ACS_CC_SUCCESS , retType);
	//ASSERT_EQUAL(1 ,imm::Base_OI::m_dataSourceCountCreated);
}
void DataSourceOITest::dataSourceDeleteSuccess() {
	std::string objName("dataSourceId=VCHS1");
	ACS_APGCC_OiHandle oiHandle = OI_HANDLE_FOR_DELETE;
	ACS_APGCC_CcbId ccbId = (ACS_APGCC_CcbId)006;

	dataSourceImpl.deleted(oiHandle,ccbId,objName.c_str());
	ACS_CC_ReturnType retType=dataSourceImpl.complete(oiHandle,ccbId);
	dataSourceImpl.apply(oiHandle,ccbId);
	ASSERT_EQUAL(ACS_CC_SUCCESS , retType);
	//ASSERT_EQUAL(0 ,imm::Base_OI::m_dataSourceCountCreated);
}

cute::suite DataSourceOITest::make_suite_dataSourceOITest(){
	cute::suite s;
	s.push_back(CUTE(DataSourceOITest::dataSourceAddSuccess));
	s.push_back(CUTE(DataSourceOITest::dataSourceAddAbort));
	s.push_back(CUTE(DataSourceOITest::dataSourceModifySuccess));
	s.push_back(CUTE(DataSourceOITest::dataSourceModifyAbort));
	s.push_back(CUTE(DataSourceOITest::dataSourceDeleteAbort));
	s.push_back(CUTE(DataSourceOITest::dataSourceDeleteSuccess));
	return s;
}
