/**
@file
UEFI OS based application.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SpdmUnitFuzzing.h"
#include "ToolChainHarness.h"
#include <SpdmRequesterLibInternal.h>

UINTN
EFIAPI
GetMaxBufferSize (
  VOID
  )
{
  return MAX_SPDM_MESSAGE_BUFFER_SIZE;
}

RETURN_STATUS
EFIAPI
SpdmDeviceSendMessage (
  IN     VOID                    *SpdmContext,
  IN     UINT32                  *SessionId,
  IN     UINTN                   RequestSize,
  IN     VOID                    *Request,
  IN     UINT64                  Timeout
  )
{
  return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
SpdmDeviceReceiveMessage (
  IN     VOID                    *SpdmContext,
     OUT UINT32                  **SessionId,
  IN OUT UINTN                   *ResponseSize,
  IN OUT VOID                    *Response,
  IN     UINT64                  Timeout
  )
{
  SPDM_TEST_CONTEXT       *SpdmTestContext;

  SpdmTestContext = GetSpdmTestContext ();
  *SessionId = NULL;
  *ResponseSize = SpdmTestContext->TestBufferSize;
  *ResponseSize = ALIGN_VALUE (*ResponseSize, SpdmTestContext->SpdmContext.Alignment);
  CopyMem (Response, SpdmTestContext->TestBuffer, SpdmTestContext->TestBufferSize);

  return RETURN_SUCCESS;
}

VOID TestSpdmRequesterGetVersion (VOID **State) {
  SPDM_TEST_CONTEXT    *SpdmTestContext;
  SPDM_DEVICE_CONTEXT  *SpdmContext;
  UINT8                VersionNumberEntryCount;
  SPDM_VERSION_NUMBER  VersionNumberEntry[MAX_SPDM_VERSION_COUNT];

  SpdmTestContext = *State;
  SpdmContext = &SpdmTestContext->SpdmContext;

  VersionNumberEntryCount = MAX_SPDM_VERSION_COUNT;
  ZeroMem (VersionNumberEntry, sizeof(VersionNumberEntry));
  SpdmGetVersion (SpdmContext, &VersionNumberEntryCount, VersionNumberEntry);
}

SPDM_TEST_CONTEXT       mSpdmRequesterGetVersionTestContext = {
  SPDM_TEST_CONTEXT_SIGNATURE,
  TRUE,
  SpdmDeviceSendMessage,
  SpdmDeviceReceiveMessage,
};

VOID
EFIAPI
RunTestHarness(
  IN VOID  *TestBuffer,
  IN UINTN TestBufferSize
  )
{
  VOID  *State;

  SetupSpdmTestContext (&mSpdmRequesterGetVersionTestContext);

  mSpdmRequesterGetVersionTestContext.TestBuffer = TestBuffer;
  mSpdmRequesterGetVersionTestContext.TestBufferSize = TestBufferSize;

  SpdmUnitTestGroupSetup (&State);

  TestSpdmRequesterGetVersion (&State);

  SpdmUnitTestGroupTeardown (&State);
}

