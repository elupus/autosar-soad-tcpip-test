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

#define CATB_NIBBLE_ERROR 0xffu
#define CATB_NIBBLE_DELIM 0xfeu

uint8 Catb_Nibble(uint8 c)
{
    if ((c >= (uint8)'0') && (c <= (uint8)'9')) {
        return c - (uint8)'0';
    }
    if ((c >= (uint8)'a') && (c <= (uint8)'f')) {
        return c - (uint8)'a' + 10u;
    }
    if ((c >= (uint8)'A') && (c <= (uint8)'F')) {
        return c - (uint8)'A' + 10u;
    }
    if ((c == (uint8)'\n') || (c == (uint8)'\r')) {
        return CATB_NIBBLE_DELIM;
    }
    return CATB_NIBBLE_ERROR;
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
        const uint8 data = Catb_Nibble(info->SduDataPtr[idx]);
        switch(status->state) {
            case CATB_IDLE:
                if (data == CATB_NIBBLE_ERROR) {
                    status->state = CATB_SKIP;
                } else if (data == CATB_NIBBLE_DELIM) {
                    status->state = CATB_IDLE;
                } else {
                    res = CATB_UP_STARTOFRECEPTION(id, &info2, 0, &status->space);
                    if (res == BUFREQ_OK) {
                        status->last  = data;
                        status->state = CATB_REM;
                    } else {
                        status->space = CATB_SKIPBUFFER_LENGTH;
                        status->state = CATB_SKIP;
                    }
                }

                break;

            case CATB_STD:
                if (data == CATB_NIBBLE_ERROR) {
                    CATB_UP_RXINDICATION(id, E_NOT_OK);
                    status->state = CATB_SKIP;
                } else if (data == CATB_NIBBLE_DELIM) {
                    CATB_UP_RXINDICATION(id, E_OK);
                    status->state = CATB_IDLE;
                } else {
                    status->last  = data;
                    status->state = CATB_REM;
                }
                break;

            case CATB_REM:
                if (data == CATB_NIBBLE_ERROR) {
                    CATB_UP_RXINDICATION(id, E_NOT_OK);
                    status->state = CATB_SKIP;
                } else if (data == CATB_NIBBLE_DELIM) {
                    CATB_UP_RXINDICATION(id, E_NOT_OK);
                    status->state = CATB_IDLE;
                } else {
                    buf = (status->last << 4u) | data;
                    res = CATB_UP_COPYRXDATA(id, &info2, &status->space);
                    if (res == BUFREQ_OK) {
                        status->state = CATB_STD;
                    } else {
                        CATB_UP_RXINDICATION(id, E_NOT_OK);
                        status->space = CATB_SKIPBUFFER_LENGTH;
                        status->state = CATB_SKIP;
                    }
                }
                break;

            case CATB_SKIP:
                if (data == CATB_NIBBLE_DELIM) {
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
