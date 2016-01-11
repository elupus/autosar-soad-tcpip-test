#include "Std_Types.h"
#include "ComStack_Types.h"
#include "Catb.h"
#include "Catb_Cfg.h"

typedef enum {
    CATB_IDLE,
    CATB_STD,
    CATB_REM,
    CATB_SKIP,
} Catb_StateType;

typedef struct {
    Catb_StateType state;
    uint8          last;
    PduLengthType  space;
} Catb_RxStatusType;

Catb_RxStatusType Catb_RxStatus[CATB_RXPDU_COUNT];

uint8 Catb_HexTo8u(uint8 h, uint8 l)
{
    return ((h - (uint8)'0') << 4u)
         | ((l - (uint8)'0'));
}

boolean Catb_IsDelimiter(uint8 data)
{
    return ((data == '\r') || (data == '\n')) ? TRUE : FALSE;
}

boolean Catb_IsNumeric(uint8 data)
{
    return ((data >= (uint8)'0')
        &&  (data <= (uint8)'9')) ? TRUE : FALSE;
}

#define CATB_SKIPBUFFER_LENGTH 0x1000u

BufReq_ReturnType Catb_StartOfReception(
        PduIdType               id,
        const PduInfoType*      info,
        PduLengthType           len,
        PduLengthType*          buf_len
    )
{
    BufReq_ReturnType res;
    PduInfoType       info2;
    Catb_RxStatusType* status = &Catb_RxStatus[id];

    info2.SduDataPtr = NULL_PTR;
    info2.SduLength  = 0;

    res = CATB_UP_STARTOFRECEPTION(id, &info2, 0, &status->space);
    if (res == BUFREQ_OK) {
        status->state = CATB_STD;
    } else {
        status->space = CATB_SKIPBUFFER_LENGTH;
        status->state = CATB_SKIP;
    }

    *buf_len = status->space * 2u;
    return res;
}

BufReq_ReturnType Catb_CopyRxData(
        PduIdType               id,
        const PduInfoType*      info,
        PduLengthType*          len
    )
{
    PduLengthType      idx, buf_len;
    BufReq_ReturnType  res;
    uint8              buf;
    PduInfoType        info2;
    Catb_RxStatusType* status = &Catb_RxStatus[id];

    info2.SduDataPtr = &buf;
    info2.SduLength  = 1u;

    for (idx = 0u; idx < info->SduLength; ++idx) {
        const uint8 data = info->SduDataPtr[idx];
        switch(status->state) {
            case CATB_IDLE:
                if (Catb_IsNumeric(data)) {

                    res = CATB_UP_STARTOFRECEPTION(id, &info2, 0, &status->space);
                    if (res == BUFREQ_OK) {
                        status->last  = data;
                        status->state = CATB_REM;
                    } else {
                        status->space = CATB_SKIPBUFFER_LENGTH;
                        status->state = CATB_SKIP;
                    }

                } else if (Catb_IsDelimiter(data)) {
                    status->state = CATB_IDLE;
                } else {
                    status->state = CATB_SKIP;
                }
                break;

            case CATB_STD:
                if (Catb_IsNumeric(data)) {
                    status->last  = data;
                    status->state = CATB_REM;
                } else if (Catb_IsDelimiter(data) == TRUE) {
                    CATB_UP_RXINDICATION(id, E_OK);
                    status->state = CATB_IDLE;
                } else {
                    CATB_UP_RXINDICATION(id, E_NOT_OK);
                    status->state = CATB_SKIP;
                }
                break;

            case CATB_REM:
                if (Catb_IsNumeric(data)) {
                    buf = Catb_HexTo8u(status->last, data);
                    res = CATB_UP_COPYRXDATA(id, &info2, &status->space);
                    if (res == BUFREQ_OK) {
                        status->state = CATB_STD;
                    } else {
                        CATB_UP_RXINDICATION(id, E_NOT_OK);
                        status->space = CATB_SKIPBUFFER_LENGTH;
                        status->state = CATB_SKIP;
                    }
                } else if (Catb_IsDelimiter(data)) {
                    CATB_UP_RXINDICATION(id, E_NOT_OK);
                    status->state = CATB_IDLE;
                } else {
                    CATB_UP_RXINDICATION(id, E_NOT_OK);
                    status->state = CATB_SKIP;
                }
                break;

            case CATB_SKIP:
                if (data == '\n') {
                    status->state = CATB_IDLE;
                }
                break;
        }
    }

    if (status->state == CATB_REM) {
        *len = status->space * 2u;
    } else {
        *len = status->space * 2u - 1u;
    }

    return BUFREQ_OK;
}

void Catb_RxIndication(
        PduIdType               id,
        Std_ReturnType          result
    )
{
    Catb_RxStatusType* status = &Catb_RxStatus[id];
    switch(status->state) {
        case CATB_STD:
        case CATB_REM:
            CATB_UP_RXINDICATION(id, E_NOT_OK);
            break;
        case CATB_IDLE:
        case CATB_SKIP:
        default:
            break;
    }
}
