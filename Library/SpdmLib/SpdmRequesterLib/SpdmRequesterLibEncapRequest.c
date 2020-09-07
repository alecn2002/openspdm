/** @file
  SPDM common library.
  It follows the SPDM Specification.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SpdmRequesterLibInternal.h"


typedef struct {
  UINT8                          RequestResponseCode;
  SPDM_GET_ENCAP_RESPONSE_FUNC   GetEncapResponseFunc;
} SPDM_GET_ENCAP_RESPONSE_STRUCT;

SPDM_GET_ENCAP_RESPONSE_STRUCT  mSpdmGetEncapResponseStruct[] = {
  {SPDM_GET_DIGESTS,            SpdmGetEncapResponseDigest},
  {SPDM_GET_CERTIFICATE,        SpdmGetEncapResponseCertificate},
};

RETURN_STATUS
EFIAPI
SpdmRegisterGetEncapResponseFunc (
  IN  VOID                          *Context,
  IN  SPDM_GET_ENCAP_RESPONSE_FUNC  GetEncapResponseFunc
  )
{
  SPDM_DEVICE_CONTEXT     *SpdmContext;

  SpdmContext = Context;
  SpdmContext->GetEncapResponseFunc = (UINTN)GetEncapResponseFunc;

  return RETURN_SUCCESS;
}

SPDM_GET_ENCAP_RESPONSE_FUNC
SpdmGetEncapResponseFunc (
  IN     SPDM_DEVICE_CONTEXT     *SpdmContext,
  IN     UINT8                   RequestResponseCode
  )
{
  UINTN                Index;

  for (Index = 0; Index < sizeof(mSpdmGetEncapResponseStruct)/sizeof(mSpdmGetEncapResponseStruct[0]); Index++) {
    if (RequestResponseCode == mSpdmGetEncapResponseStruct[Index].RequestResponseCode) {
      return mSpdmGetEncapResponseStruct[Index].GetEncapResponseFunc;
    }
  }
  return NULL;
}

RETURN_STATUS
SpdmProcessEncapsulatedRequest (
  IN     SPDM_DEVICE_CONTEXT  *SpdmContext,
  IN     UINTN                EncapRequestSize,
  IN     VOID                 *EncapRequest,
  IN OUT UINTN                *EncapResponseSize,
     OUT VOID                 *EncapResponse
  )
{
  SPDM_GET_ENCAP_RESPONSE_FUNC    GetEncapResponseFunc;
  RETURN_STATUS                   Status;

  GetEncapResponseFunc = SpdmGetEncapResponseFunc (SpdmContext, *(UINT8 *)EncapRequest);
  if (GetEncapResponseFunc == NULL) {
    GetEncapResponseFunc = (SPDM_GET_ENCAP_RESPONSE_FUNC)SpdmContext->GetEncapResponseFunc;
  }
  if (GetEncapResponseFunc != NULL) {
    Status = GetEncapResponseFunc (SpdmContext, EncapRequestSize, EncapRequest, EncapResponseSize, EncapResponse);
  } else {
    Status = RETURN_NOT_FOUND;
  }
  if (Status != RETURN_SUCCESS) {
    SpdmGenerateEncapErrorResponse (SpdmContext, SPDM_ERROR_CODE_UNSUPPORTED_REQUEST, *(UINT8 *)EncapRequest, EncapResponseSize, EncapResponse);
  }

  return RETURN_SUCCESS;
}

/**
  This function executes SPDM Encapsulated Request.
  
  @param[in]  SpdmContext            The SPDM context for the device.
**/
RETURN_STATUS
SpdmEncapsulatedRequest (
  IN     SPDM_DEVICE_CONTEXT  *SpdmContext,
  IN     UINT32               SessionId
  )
{
  RETURN_STATUS                               Status;
  UINT8                                       Request[MAX_SPDM_MESSAGE_BUFFER_SIZE];
  UINTN                                       SpdmRequestSize;
  SPDM_GET_ENCAPSULATED_REQUEST_REQUEST       *SpdmGetEncapsulatedRequestRequest;
  SPDM_DELIVER_ENCAPSULATED_RESPONSE_REQUEST  *SpdmDeliverEncapsulatedResponseRequest;
  UINT8                                       Response[MAX_SPDM_MESSAGE_BUFFER_SIZE];
  UINTN                                       SpdmResponseSize;
  SPDM_ENCAPSULATED_REQUEST_RESPONSE          *SpdmEncapsulatedRequestResponse;
  SPDM_ENCAPSULATED_RESPONSE_ACK_RESPONSE     *SpdmEncapsulatedResponseAckResponse;
  SPDM_SESSION_INFO                           *SessionInfo;
  UINT8                                       RequestId;
  VOID                                        *EncapsulatedRequest;
  UINTN                                       EncapsulatedRequestSize;
  VOID                                        *EncapsulatedResponse;
  UINTN                                       EncapsulatedResponseSize;
  
  if ((SpdmContext->ConnectionInfo.Capability.Flags & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCAP_CAP) == 0) {
    return RETURN_DEVICE_ERROR;
  }

  SessionInfo = SpdmGetSessionInfoViaSessionId (SpdmContext, SessionId);
  if (SessionInfo == NULL) {
    ASSERT (FALSE);
    return RETURN_UNSUPPORTED;
  }

  SpdmGetEncapsulatedRequestRequest = (VOID *)Request;
  SpdmGetEncapsulatedRequestRequest->Header.SPDMVersion = SPDM_MESSAGE_VERSION_11;
  SpdmGetEncapsulatedRequestRequest->Header.RequestResponseCode = SPDM_GET_ENCAPSULATED_REQUEST;
  SpdmGetEncapsulatedRequestRequest->Header.Param1 = 0;
  SpdmGetEncapsulatedRequestRequest->Header.Param2 = 0;
  SpdmRequestSize = sizeof(SPDM_GET_ENCAPSULATED_REQUEST_REQUEST);
  Status = SpdmSendRequestSession (SpdmContext, SessionId, SpdmRequestSize, SpdmGetEncapsulatedRequestRequest);
  if (RETURN_ERROR(Status)) {
    return RETURN_DEVICE_ERROR;
  }

  SpdmEncapsulatedRequestResponse = (VOID *)Response;
  SpdmResponseSize = sizeof(Response);
  ZeroMem (&Response, sizeof(Response));
  Status = SpdmReceiveResponseSession (SpdmContext, SessionId, &SpdmResponseSize, SpdmEncapsulatedRequestResponse);
  if (RETURN_ERROR(Status)) {
    return RETURN_DEVICE_ERROR;
  }
  if (SpdmEncapsulatedRequestResponse->Header.RequestResponseCode != SPDM_ENCAPSULATED_REQUEST) {
    return RETURN_DEVICE_ERROR;
  }
  if (SpdmResponseSize < sizeof(SPDM_ENCAPSULATED_REQUEST_RESPONSE)) {
    return RETURN_DEVICE_ERROR;
  }
  if (SpdmResponseSize == sizeof(SPDM_ENCAPSULATED_REQUEST_RESPONSE)) {
    //
    // Done
    //
    return RETURN_SUCCESS;
  }
  RequestId = SpdmEncapsulatedRequestResponse->Header.Param1;

  EncapsulatedRequest = (VOID *)(SpdmEncapsulatedRequestResponse + 1);
  EncapsulatedRequestSize = SpdmResponseSize - sizeof(SPDM_ENCAPSULATED_REQUEST_RESPONSE);

  while (TRUE) {
    //
    // Process Request
    //
    SpdmDeliverEncapsulatedResponseRequest = (VOID *)Request;
    SpdmDeliverEncapsulatedResponseRequest->Header.SPDMVersion = SPDM_MESSAGE_VERSION_11;
    SpdmDeliverEncapsulatedResponseRequest->Header.RequestResponseCode = SPDM_DELIVER_ENCAPSULATED_RESPONSE;
    SpdmDeliverEncapsulatedResponseRequest->Header.Param1 = RequestId;
    SpdmDeliverEncapsulatedResponseRequest->Header.Param2 = 0;
    EncapsulatedResponse = (VOID *)(SpdmDeliverEncapsulatedResponseRequest + 1);
    EncapsulatedResponseSize = sizeof(Request) - sizeof(SPDM_DELIVER_ENCAPSULATED_RESPONSE_REQUEST);

    Status = SpdmProcessEncapsulatedRequest (SpdmContext, EncapsulatedRequestSize, EncapsulatedRequest, &EncapsulatedResponseSize, EncapsulatedResponse);
    if (RETURN_ERROR(Status)) {
      return RETURN_DEVICE_ERROR;
    }

    SpdmRequestSize = sizeof(SPDM_DELIVER_ENCAPSULATED_RESPONSE_REQUEST) + EncapsulatedResponseSize;
    Status = SpdmSendRequestSession (SpdmContext, SessionId, SpdmRequestSize, SpdmDeliverEncapsulatedResponseRequest);
    if (RETURN_ERROR(Status)) {
      return RETURN_DEVICE_ERROR;
    }
    
    SpdmEncapsulatedResponseAckResponse = (VOID *)Response;
    SpdmResponseSize = sizeof(Response);
    ZeroMem (&Response, sizeof(Response));
    Status = SpdmReceiveResponseSession (SpdmContext, SessionId, &SpdmResponseSize, SpdmEncapsulatedResponseAckResponse);
    if (RETURN_ERROR(Status)) {
      return RETURN_DEVICE_ERROR;
    }
    if (SpdmEncapsulatedResponseAckResponse->Header.RequestResponseCode != SPDM_ENCAPSULATED_RESPONSE_ACK) {
      return RETURN_DEVICE_ERROR;
    }
    if (SpdmResponseSize < sizeof(SPDM_ENCAPSULATED_RESPONSE_ACK_RESPONSE)) {
      return RETURN_DEVICE_ERROR;
    }
    if ((SpdmEncapsulatedResponseAckResponse->Header.Param2 == SPDM_ENCAPSULATED_RESPONSE_ACK_RESPONSE_PAYLOAD_TYPE_ABSENT) &&
        (SpdmResponseSize == sizeof(SPDM_ENCAPSULATED_RESPONSE_ACK_RESPONSE))) {
      //
      // Done
      //
      break;
    }
    RequestId = SpdmEncapsulatedResponseAckResponse->Header.Param1;

    EncapsulatedRequest = (VOID *)(SpdmEncapsulatedResponseAckResponse + 1);
    EncapsulatedRequestSize = SpdmResponseSize - sizeof(SPDM_ENCAPSULATED_RESPONSE_ACK_RESPONSE);
  }

  return RETURN_SUCCESS;
}

