//
//  BREthereumNode.h
//  breadwallet-core Ethereum
//
//  Created by Lamont Samuels on 5/29/18.
//  Copyright (c) 2018 breadwallet LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#ifndef BR_Ethereum_LES_Coder_h
#define BR_Ethereum_LES_Coder_h

#include <stdio.h>
#include <inttypes.h>
#include "../base/BREthereumBase.h"
#include "../blockchain/BREthereumBlockChain.h"
#include "BRKey.h"
#include "BRInt.h"
#include "BRArray.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TXSTATUS_INCLUDED 3
#define TXSTATUS_ERROR 4

typedef enum {
  BRE_LES_ID_ANNOUNCE        = 0x01, 
  BRE_LES_ID_STATUS          = 0x00,
  BRE_LES_ID_SEND_TX2        = 0x13,
  BRE_LES_ID_SEND_TX         = 0x0c,
  BRE_LES_ID_GET_TX_STATUS   = 0x14,
  BRE_LES_ID_TX_STATUS       = 0x15,
  BRE_LES_ID_GET_BLOCK_BODIES = 0x04,
  BRE_PIP_ID_REQUEST          = 0x02,
  BRE_PIP_ID_RESPONSE         = 0x03,
  BRE_LES_ID_BLOCK_BODIES     = 0x05,
  BRE_LES_ID_GET_BLOCK_HEADERS = 0x02,
  BRE_LES_ID_BLOCK_HEADERS = 0x03,
  BRE_LES_ID_GET_RECEIPTS = 0x06,
  BRE_LES_ID_RECEIPTS = 0x07,
  BRE_LES_ID_GET_PROOFS_V2 = 0x0f,
  BRE_LES_ID_PROOFS_V2 = 0x10
}LESMessageId;

extern const char *
lesMessageGetName (LESMessageId id);

typedef enum {
    BRE_LES_CODER_SUCCESS=0,
    BRE_LES_CODER_INVALID_MSG_ID_ERROR,
    BRE_LES_CODER_INVALID_STATUS_KEY_PAIR,
    BRE_LES_CODER_UNABLE_TO_DECODE_ERROR,
}BREthereumLESDecodeStatus;

typedef struct {
    uint64_t msgCode;
    uint64_t baseCost;
    uint64_t reqCost;
}BREthereumLESMRC;


//
// LES Status Structures
//
typedef struct {
    uint64_t protocolVersion;
    uint64_t chainId;
    UInt256 headerTd;
    uint8_t headHash[32];
    uint64_t headNum;
    uint8_t genesisHash[32];
    // Note: The below fields are optional LPV1
    BREthereumBoolean serveHeaders;
    uint64_t* serveChainSince;
    uint64_t* serveStateSince;
    BREthereumBoolean txRelay;
    uint64_t*flowControlBL;
    BREthereumLESMRC*flowControlMRC;
    size_t* flowControlMRCCount;
    uint64_t*flowControlMRR;
    uint64_t announceType;
}BREthereumLESStatusMessage;

extern void
lesStatusMessageLogFlowControl (BREthereumLESStatusMessage *message);

typedef struct {
    BREthereumHash blockHash;
    BREthereumHash key;
    BREthereumHash key2;
    uint64_t fromLevel;
}BREthereumProofsRequest;

//
// LES Request Structures
//
typedef struct {
    char* key;
    union
    {
        uint64_t intValue;
        UInt256 bigIntValue;
        uint8_t byteValue;
        BREthereumBoolean boolValue;
        void* ptrValue;
    }u;
}BREthereumAnnounceRequest;


//
// Status Message
//
extern BRRlpData ethereumLESEncodeStatus(uint64_t message_id_offset, BREthereumLESStatusMessage* status);
extern BREthereumLESDecodeStatus ethereumLESDecodeStatus(uint8_t*rlpBytes, size_t rlpBytesSize, BREthereumLESStatusMessage* status);

//
// Transaction relaying and status retrieval
//
extern BRRlpData ethereumLESSendTxt(uint64_t message_id_offset, uint64_t reqId, BREthereumTransaction transactions[], BREthereumNetwork network, BREthereumRlpType type);
extern BRRlpData ethereumLESSendTxtV2(uint64_t message_id_offset, uint64_t reqId, BREthereumTransaction transactions[], BREthereumNetwork network, BREthereumRlpType type);

//
// Header synchronisation
//
extern BRRlpData ethereumLESGetBlockHeaders(uint64_t message_id_offset,
                                      uint64_t reqId,
                                      uint64_t block,
                                      uint64_t maxHeaders,
                                      uint64_t skip,
                                      uint64_t reverse);

BREthereumLESDecodeStatus ethereumLESDecodeAnnounce(uint8_t*rlpBytes, size_t rlpBytesSize,
                                                    BREthereumHash* hash,
                                                    uint64_t* headNumber, UInt256* headTd, uint64_t* reorgDepth,
                                                    BREthereumLESStatusMessage* message);
    
BREthereumLESDecodeStatus ethereumLESDecodeBlockHeaders(uint8_t*rlpBytes, size_t rlpBytesSize,  uint64_t* reqId, uint64_t* bv,
                                   BREthereumBlockHeader** blockHeaders);
    
 //
// On-demand data retrieval
//
extern BRRlpData ethereumLESGetBlockBodies(uint64_t message_id_offset, uint64_t reqId, BREthereumHash* blockHashes);
extern BREthereumLESDecodeStatus ethereumLESDecodeBlockBodies(uint8_t*rlpBytes, size_t rlpBytesSize, uint64_t* reqId, uint64_t* bv, BREthereumNetwork network, BREthereumBlockHeader***ommers,  BREthereumTransaction***transactions);

extern BRRlpData ethereumLESGetReceipts(uint64_t message_id_offset, uint64_t reqId, BREthereumHash* blockHashes);
extern BREthereumLESDecodeStatus ethereumLESDecodeReceipts(uint8_t*rlpBytes, size_t rlpBytesSize, uint64_t* reqId, uint64_t* bv, BREthereumTransactionReceipt***receipts);


extern BRRlpData ethereumLESGetProofsV2(uint64_t message_id_offset, uint64_t reqId, BREthereumProofsRequest* requests);




extern BRRlpData ethereumLESGetTxStatus(uint64_t message_id_offset, uint64_t reqId, BREthereumHash* transactions);
extern BREthereumLESDecodeStatus ethereumLESDecodeTxStatus(uint8_t*rlpBytes, size_t rlpBytesSize, uint64_t* reqId, uint64_t* bv, BREthereumTransactionStatus** replies, size_t* repliesCount);


//
// Pip (Parity) related coder funcutions
//

typedef struct {
//        [U8](U8) // merkle inclusion proof from state trie
//        U // nonce
//        U // balance
    uint8_t* merkleProofState;
    uint64_t nonce;
    uint64_t balance;
    UInt256 codeHash;
    UInt256 storageRoot; // storage root

} BREthereumPipResponse;

extern BRRlpData ethereumPipAccountRequest(uint64_t message_id_offset, uint64_t reqId, BREthereumHash block, BREthereumAddress address);
extern BREthereumLESDecodeStatus ethereumPipDecodeAccountResponse(uint8_t*rlpBytes, size_t rlpBytesSize, uint64_t* reqId, uint64_t* cr, BREthereumPipResponse* response);

#ifdef __cplusplus
}
#endif

#endif /* BR_Ethereum_LES_Coder_h */
