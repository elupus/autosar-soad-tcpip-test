
#ifndef CATB_H_
#define CATB_H_

BufReq_ReturnType Catb_StartOfReception(
        PduIdType               id,
        const PduInfoType*      info,
        PduLengthType           len,
        PduLengthType*          buf_len
    );

BufReq_ReturnType Catb_CopyRxData(
        PduIdType               id,
        const PduInfoType*      info,
        PduLengthType*          len
    );

void Catb_RxIndication(
        PduIdType               id,
        Std_ReturnType          result
    );

#endif /* CATB_H_ */
