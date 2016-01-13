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

typedef struct {
    PduLengthType  remain;
} Catb_TxStatusType;

Catb_RxStatusType Catb_RxStatus[CATB_RXPDU_COUNT];
Catb_TxStatusType Catb_TxStatus[CATB_TXPDU_COUNT];

#define CATB_NIBBLE_ERROR 0xffu
#define CATB_NIBBLE_DELIM 0xfeu

static uint8 Catb_Nibble_Decode(uint8 c)
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

static uint8 Catb_Nibble_Encode(uint8 src)
{
    if(src <= 0x9) {
        return (uint8)'0' + src;
    } else {
        return (uint8)'a' + src;
    }
}

static const uint8 Catb_Suffix[2]  = {(uint8)'\n', (uint8)'\r'};


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
        const uint8 data = Catb_Nibble_Decode(info->SduDataPtr[idx]);
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

Std_ReturnType Catb_Transmit(
        PduIdType               id,
        const PduInfoType*      info
    )
{
    BufReq_ReturnType  res;
    PduInfoType        info2;
    Catb_TxStatusType* status = &Catb_TxStatus[id];
    info2.SduDataPtr = NULL_PTR;
    info2.SduLength  = info->SduLength * 2u + sizeof(Catb_Suffix);
    res = CATB_LO_TRANSMIT(id, info2);
    if (res == E_OK) {
        status->remain = info2.SduLength;
    } else {
        status->remain = 0u;
    }
    return res;
}

BufReq_ReturnType Catb_CopyTxData(
        PduIdType               id,
        const PduInfoType*      info,
        RetryInfoType*          retry,
        PduLengthType*          available
    )
{
    PduLengthType      trg = 0u;
    PduInfoType        info2;
    Catb_TxStatusType* status = &Catb_TxStatus[id];

    if (info->SduLength & 0x1u) {
        return BUFREQ_E_NOT_OK;
    }

    if (status->remain > sizeof(Catb_Suffix)) {
        PduLengthType      rem;
        BufReq_ReturnType  res;

        if (status->remain - sizeof(Catb_Suffix) > info->SduLength) {
            info2.SduLength = info->SduLength / 2u;
        } else {
            info2.SduLength = status->remain - sizeof(Catb_Suffix);
        }
        info2.SduDataPtr = &info->SduDataPtr[info2.SduLength];

        res = CATB_UP_COPYTXDATA(id, &info2, retry, &rem);
        if (res == BUFREQ_OK) {
            PduLengthType      idx;

            for(idx = 0u; idx < info2.SduLength; ++idx, trg += 2u) {
                info->SduDataPtr[trg+0u] = Catb_Nibble_Encode(info2.SduDataPtr[idx] >> 4u);
                info->SduDataPtr[trg+1u] = Catb_Nibble_Encode(info2.SduDataPtr[idx] & 0xfu);
            }
            status->remain = rem * 2u + sizeof(Catb_Suffix);
        } else {
            return res;
        }
    }

    if (status->remain <= sizeof(Catb_Suffix)) {
        while (trg < info->SduLength) {
            info->SduDataPtr[trg] = Catb_Suffix[status->remain];
            trg++;
            status->remain--;
        }
    }

    return E_OK;
}

void Catb_TxConfirmation(
        PduIdType               id,
        Std_ReturnType          result
    )
{
    CATB_UP_TXCONFIRMATION(id, result);
}
