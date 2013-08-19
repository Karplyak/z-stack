#ifndef AF_H
#define AF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ZComDef.h"
#include "nwk.h"
#include "APSMEDE.h"

#define AF_BROADCAST_ENDPOINT              0xFF
#define AF_ACK_REQUEST                     0x10
#define AF_DISCV_ROUTE                     0x20
#define AF_EN_SECURITY                     0x40
#define AF_SKIP_ROUTING                    0x80

// Backwards support for afAddOrSendMessage / afFillAndSendMessage.
#define AF_TX_OPTIONS_NONE                 0
#define AF_MSG_ACK_REQUEST                 AF_ACK_REQUEST

// Default Radius Count value
#define AF_DEFAULT_RADIUS                  DEF_NWK_RADIUS

#define AF_MAX_USER_DESCRIPTOR_LEN         16
#define AF_USER_DESCRIPTOR_FILL          0x20
typedef struct
{
  byte len;     // Length of string descriptor
  byte desc[AF_MAX_USER_DESCRIPTOR_LEN];
} UserDescriptorFormat_t;

// Node Logical Types
#define NODETYPE_COORDINATOR    0x00
#define NODETYPE_ROUTER         0x01
#define NODETYPE_DEVICE         0x02

// Node Frequency Band - bit map
#define NODEFREQ_800            0x01    // 868 - 868.6 MHz
#define NODEFREQ_900            0x04    // 902 - 928 MHz
#define NODEFREQ_2400           0x08    // 2400 - 2483.5 MHz

// Node Descriptor format structure
typedef struct
{
  uint8 LogicalType:3;
  uint8 ComplexDescAvail:1;  /* AF_V1_SUPPORT - reserved bit. */
  uint8 UserDescAvail:1;     /* AF_V1_SUPPORT - reserved bit. */
  uint8 Reserved:3;
  uint8 APSFlags:3;
  uint8 FrequencyBand:5;
  uint8 CapabilityFlags;
  uint8 ManufacturerCode[2];
  uint8 MaxBufferSize;
  uint8 MaxInTransferSize[2];
  uint16 ServerMask;
  uint8 MaxOutTransferSize[2];
  uint8 DescriptorCapability; 
} NodeDescriptorFormat_t;

// Bit masks for the ServerMask.
#define PRIM_TRUST_CENTER  0x01
#define BKUP_TRUST_CENTER  0x02
#define PRIM_BIND_TABLE    0x04
#define BKUP_BIND_TABLE    0x08
#define PRIM_DISC_TABLE    0x10
#define BKUP_DISC_TABLE    0x20
#define NETWORK_MANAGER    0x40

// Node Current Power Modes (CURPWR)
// Receiver permanently on or sync with coordinator beacon.
#define NODECURPWR_RCVR_ALWAYS_ON   0x00
// Receiver automatically comes on periodically as defined by the
// Node Power Descriptor.
#define NODECURPWR_RCVR_AUTO        0x01
// Receiver comes on when simulated, eg by a user pressing a button.
#define NODECURPWR_RCVR_STIM        0x02

// Node Available Power Sources (AVAILPWR) - bit map
//   Can be used for AvailablePowerSources or CurrentPowerSource
#define NODEAVAILPWR_MAINS          0x01  // Constant (Mains) power
#define NODEAVAILPWR_RECHARGE       0x02  // Rechargeable Battery
#define NODEAVAILPWR_DISPOSE        0x04  // Disposable Battery

// Power Level
#define NODEPOWER_LEVEL_CRITICAL    0x00  // Critical
#define NODEPOWER_LEVEL_33          0x04  // 33%
#define NODEPOWER_LEVEL_66          0x08  // 66%
#define NODEPOWER_LEVEL_100         0x0C  // 100%

// Node Power Descriptor format structure
typedef struct
{
  unsigned int PowerMode:4;
  unsigned int AvailablePowerSources:4;
  unsigned int CurrentPowerSource:4;
  unsigned int CurrentPowerSourceLevel:4;
} NodePowerDescriptorFormat_t;

// AppDevVer values
#define APPDEVVER_1               0x01

// AF_V1_SUPPORT AppFlags - bit map
#define APPFLAG_NONE                0x00  // Backwards compatibility to AF_V1.

// AF-AppFlags - bit map
#define AF_APPFLAG_NONE             0x00
#define AF_APPFLAG_COMPLEXDESC      0x01  // Complex Descriptor Available
#define AF_APPFLAG_USERDESC         0x02  // User Descriptor Available

typedef uint16  cId_t;
// Simple Description Format Structure
typedef struct
{
  byte          EndPoint;
  uint16        AppProfId;
  uint16        AppDeviceId;
  byte          AppDevVer:4;
  byte          Reserved:4;             // AF_V1_SUPPORT uses for AppFlags:4.
  byte          AppNumInClusters;
  cId_t         *pAppInClusterList;
  byte          AppNumOutClusters;
  cId_t         *pAppOutClusterList;
} SimpleDescriptionFormat_t;

// Frame Types
#define FRAMETYPE_KVP          0x01     // 0001
#define FRAMETYPE_MSG          0x02     // 0010

#define ERRORCODE_SUCCESS               0x00

#define AF_HDR_KVP_MAX_LEN   0x08  // Max possible AF KVP header.
#define AF_HDR_V1_0_MAX_LEN  0x03  // Max possible AF Ver 1.0 header.
#define AF_HDR_V1_1_MAX_LEN  0x00  // Max possible AF Ver 1.1 header.

// Generalized MSG Command Format
typedef struct
{
  byte   TransSeqNumber;
  uint16 DataLength;               // Number of bytes in TransData
  byte  *Data;
} afMSGCommandFormat_t;

typedef enum
{
  noLatencyReqs,
  fastBeacons,
  slowBeacons
} afNetworkLatencyReq_t;

typedef enum
{
  afAddrNotPresent = AddrNotPresent,
  afAddr16Bit      = Addr16Bit,
  afAddr64Bit      = Addr64Bit,
  afAddrGroup      = AddrGroup,
  afAddrBroadcast  = AddrBroadcast
} afAddrMode_t;

typedef struct
{
  union
  {
    uint16      shortAddr;
    ZLongAddr_t extAddr;
  } addr;
  afAddrMode_t addrMode;
  byte endPoint;
  uint16 panId;  // used for the INTER_PAN feature
} afAddrType_t;

typedef struct
{
  osal_event_hdr_t hdr;     /* OSAL Message header */
  uint16 groupId;           /* Message's group ID - 0 if not set */
  uint16 clusterId;         /* Message's cluster ID */
  afAddrType_t srcAddr;     /* Source Address, if endpoint is STUBAPS_INTER_PAN_EP,
                               it's an InterPAN message */
  uint16 macDestAddr;       /* MAC header destination short address */
  uint8 endPoint;           /* destination endpoint */
  uint8 wasBroadcast;       /* TRUE if network destination was a broadcast address */
  uint8 LinkQuality;        /* The link quality of the received data frame */
  uint8 correlation;        /* The raw correlation value of the received data frame */
  int8  rssi;               /* The received RF power in units dBm */
  uint8 SecurityUse;        /* deprecated */
  uint32 timestamp;         /* receipt timestamp from MAC */
  afMSGCommandFormat_t cmd; /* Application Data */
} afIncomingMSGPacket_t;

typedef struct
{
  osal_event_hdr_t hdr;
  byte endpoint;
  byte transID;
} afDataConfirm_t;

typedef struct
{
  byte endPoint;
  byte *task_id;  // Pointer to location of the Application task ID.
  SimpleDescriptionFormat_t *simpleDesc;
  afNetworkLatencyReq_t latencyReq;
} endPointDesc_t;

typedef void *(*pDescCB)( uint8 type, uint8 endpoint );

// Descriptor types used in the above callback
#define AF_DESCRIPTOR_SIMPLE            1
#define AF_DESCRIPTOR_PROFILE_ID        2

// Bit definitions for epList_t flags.
typedef enum
{
  eEP_AllowMatch = 1,
  eEP_NotUsed
} eEP_Flags;

typedef struct
{
  endPointDesc_t *epDesc;
  eEP_Flags flags;
  pDescCB  pfnDescCB;     // Don't use if this function pointer is NULL.
  void *nextDesc;
} epList_t;

#define afStatus_SUCCESS            ZSuccess
#define afStatus_FAILED             ZFailure
#define afStatus_MEM_FAIL           ZMemError
#define afStatus_INVALID_PARAMETER  ZInvalidParameter

typedef ZStatus_t afStatus_t;

typedef struct
{
  uint8              kvp;
  APSDE_DataReqMTU_t aps;
} afDataReqMTU_t;

//[Type][Logical address] [Module/M140,M160][Data]
//[1Byte][4Bytes][3Bytes][8Bytes] =>[16Bytes]
typedef struct
{
  char Type[8];
//  uint32 *shortAddr;   //HalUARTWrite(uint8 port, uint8 *buf, uint16 len)
  char Module[10];  
  byte Data[30];        // receive data array  
  uint16 DataLength;    // Number of bytes in TransData
} packet_t;
  
extern epList_t *epList;
extern packet_t device_manager;

extern void afInit( void );
extern epList_t *afRegisterExtended( endPointDesc_t *epDesc, pDescCB descFn );
extern afStatus_t afRegister( endPointDesc_t *epDesc );
extern void afDataConfirm( uint8 endPoint, uint8 transID, ZStatus_t status );
extern void afIncomingData( aps_FrameFormat_t *aff, zAddrType_t *SrcAddress, uint16 SrcPanId,
                       NLDE_Signal_t *sig, byte SecurityUse, uint32 timestamp );

afStatus_t AF_DataRequest( afAddrType_t *dstAddr, endPointDesc_t *srcEP,
                             uint16 cID, uint16 len, uint8 *buf, uint8 *transID,
                             uint8 options, uint8 radius );

  
/*********************************************************************
 * @fn      AF_DataRequestSrcRtg
 *
 * @brief   Common functionality for invoking APSDE_DataReq() for both
 *          SendMulti and MSG-Send.
 *
 * input parameters
 *
 * @param  *dstAddr - Full ZB destination address: Nwk Addr + End Point.
 * @param  *srcEP - Origination (i.e. respond to or ack to) End Point Descr.
 * @param   cID - A valid cluster ID as specified by the Profile.
 * @param   len - Number of bytes of data pointed to by next param.
 * @param  *buf - A pointer to the data bytes to send.
 * @param  *transID - A pointer to a byte which can be modified and which will
 *                    be used as the transaction sequence number of the msg.
 * @param   options - Valid bit mask of Tx options.
 * @param   radius - Normally set to AF_DEFAULT_RADIUS.
 * @param   relayCnt - Number of devices in the relay list
 * @param   pRelayList - Pointer to the relay list
 *
 * output parameters
 *
 * @param  *transID - Incremented by one if the return value is success.
 *
 * @return  afStatus_t - See previous definition of afStatus_... types.
 */

afStatus_t AF_DataRequestSrcRtg( afAddrType_t *dstAddr, endPointDesc_t *srcEP,
                           uint16 cID, uint16 len, uint8 *buf, uint8 *transID,
                           uint8 options, uint8 radius, uint8 relayCnt, 
                           uint16* pRelayList );

extern endPointDesc_t *afFindEndPointDesc( byte endPoint );
extern byte afFindSimpleDesc( SimpleDescriptionFormat_t **ppDesc, byte EP );
extern uint8 afDataReqMTU( afDataReqMTU_t* fields );
extern uint8 afGetMatch( uint8 ep );
extern uint8 afSetMatch( uint8 ep, uint8 action );
extern byte afNumEndPoints( void );
extern void afEndPoints( byte *epBuf, byte skipZDO );
extern void afCopyAddress (afAddrType_t *afAddr, zAddrType_t *zAddr);

#ifdef __cplusplus
}
#endif

#endif