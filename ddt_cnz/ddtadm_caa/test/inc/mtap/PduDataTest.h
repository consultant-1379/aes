#ifndef INC_MTAP_PDUDATATEST_H_
#define INC_MTAP_PDUDATATEST_H_

#include "cute_suite.h"

void createPutRecordPduTest();
void createDoWillPduTest();
void createGetParamPduTest();
void createUnlinkAllFilesPduTest();
void createUnlinkFilesPduTest();
void createInvalidPduTest();
void createSynchronizePduTest();

extern cute::suite make_suite_pduHandlerTest();
#endif //INC_MTAP_PDUDATATEST_H_
