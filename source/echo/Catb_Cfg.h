
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
    PduLengthType*          len
);

extern void Main_CatbRxIndication(
    PduIdType               id,
    Std_ReturnType          result
);

#define CATB_RXPDU_COUNT 1u

#define CATB_UP_STARTOFRECEPTION Main_CatbStartOfReception
#define CATB_UP_COPYRXDATA       Main_CatbCopyRxData
#define CATB_UP_RXINDICATION     Main_CatbRxIndication

#endif /* CATB_CFG_H_ */
