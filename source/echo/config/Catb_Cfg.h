
#ifndef CATB_CFG_H_
#define CATB_CFG_H_

extern BufReq_ReturnType Main_CatbStartOfReception(
    PduIdType               id,
    const PduInfoType*      info,
    PduLengthType           len,
    PduLengthType*          buf_len
);

extern BufReq_ReturnType Main_CatbCopyRxData(
    PduIdType               id,
    const PduInfoType*      info,
    PduLengthType*          buf_len
);

extern void Main_CatbRxIndication(
    PduIdType               id,
    Std_ReturnType          result
);


BufReq_ReturnType Main_CatbCopyTxData(
        PduIdType               id,
        const PduInfoType*      info,
        RetryInfoType*          retry,
        PduLengthType*          available
    );

void Main_CatbTxConfirmation(
        PduIdType               id,
        Std_ReturnType          result
    );

#define CATB_RXPDU_COUNT 1u
#define CATB_TXPDU_COUNT 1u

#define CATB_UP_STARTOFRECEPTION(id, info, len, buf_len) Main_CatbStartOfReception(id, info, len, buf_len)
#define CATB_UP_COPYRXDATA(id, info, buf_len)            Main_CatbCopyRxData(id, info, buf_len)
#define CATB_UP_RXINDICATION(id, result)                 Main_CatbRxIndication(id, result)

#define CATB_UP_COPYTXDATA(id, info, retry, available)   Main_CatbCopyTxData(id, info, retry, available)
#define CATB_UP_TXCONFIRMATION(id, result)               Main_CatbTxConfirmation(id, result)

#define CATB_LO_TRANSMIT(id, info)                         (E_NOT_OK)

#endif /* CATB_CFG_H_ */
