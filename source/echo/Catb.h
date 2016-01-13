
#ifndef CATB_H_
#define CATB_H_

#include "Std_Types.h"
#include "ComStack_Types.h"

BufReq_ReturnType Catb_StartOfReception(
        PduIdType               id,
        const PduInfoType*      info,
        PduLengthType           len,
        PduLengthType*          buf_len
    );

BufReq_ReturnType Catb_CopyRxData(
        PduIdType               id,
        const PduInfoType*      info,
        PduLengthType*          buf_len
    );

void Catb_RxIndication(
        PduIdType               id,
        Std_ReturnType          result
    );

Std_ReturnType Catb_Transmit(
        PduIdType               id,
        const PduInfoType*      info
    );

BufReq_ReturnType Catb_CopyTxData(
        PduIdType               id,
        const PduInfoType*      info,
        RetryInfoType*          retry,
        PduLengthType*          available
    );

void Catb_TxConfirmation(
        PduIdType               id,
        Std_ReturnType          result
    );

#endif /* CATB_H_ */
