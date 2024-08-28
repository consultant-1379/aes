#include "cute.h"

#include "imm/transferprogressOITest.h"
#include "stubs/imm_stubs.h"
#include "imm/imm.h"
#include <iostream>
#include <string>

imm::TransferProgress_OI TransferProgressOITest::transferProgressImpl(imm::moc_name::TRANSFER_PROGRESS, imm::oi_name::TRANSFER_PROGRESS);

// success case
void TransferProgressOITest::updateRuntimeSuccess() {
	std::string objName("transferProgressId=BC0,AxeDirectDataTransferdirectDataTransferMId=1,dataSourceId=VCHS");
	int numOfAttributes = 2;
	char * attrNames[numOfAttributes];

	std::string attrName1 = imm::transferprogress_attribute::LASTRECEIVEDMESSAGE;
	size_t len = attrName1.size();
	attrNames[0] = new char[len+1];
	memset(attrNames[0],0,len+1);
	strncpy(attrNames[0],attrName1.c_str(),len+1);

	std::string attrName2 = imm::transferprogress_attribute::LASTSENTMESSAGE;
	size_t len1 = attrName2.size();
	attrNames[1] = new char[len1+1];
	memset(attrNames[1],0,len1+1);
	strncpy(attrNames[1],attrName2.c_str(),len1+1);
	ACS_CC_ReturnType retType=transferProgressImpl.updateRuntime(objName.c_str(),(const char **)attrNames);
	ASSERT_EQUAL(ACS_CC_SUCCESS , retType);

}
// failure case
void TransferProgressOITest::updateRuntimeFailure() {
	std::string objName("transferProgressId=dummy,AxeDirectDataTransferdirectDataTransferMId=1,dataSourceId=VCHS");
	int numOfAttributes = 2;
	char * attrNames[numOfAttributes];

	std::string attrName1 = imm::transferprogress_attribute::LASTRECEIVEDMESSAGE;
	size_t len = attrName1.size();
	attrNames[0] = new char[len+1];
	memset(attrNames[0],0,len+1);
	strncpy(attrNames[0],attrName1.c_str(),len+1);

	std::string attrName2 = imm::transferprogress_attribute::LASTSENTMESSAGE;
	size_t len1 = attrName2.size();
	attrNames[1] = new char[len1+1];
	memset(attrNames[1],0,len1+1);
	strncpy(attrNames[1],attrName2.c_str(),len1+1);

	ACS_CC_ReturnType retType=transferProgressImpl.updateRuntime(objName.c_str(),(const char**)attrNames);
	ASSERT_EQUAL(ACS_CC_FAILURE , retType);

}

cute::suite TransferProgressOITest::make_suite_transferProgressOITest(){
	cute::suite s;
	s.push_back(CUTE(TransferProgressOITest::updateRuntimeSuccess));
	s.push_back(CUTE(TransferProgressOITest::updateRuntimeFailure));
	return s;
}
