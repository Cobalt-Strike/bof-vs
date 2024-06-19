#ifndef _BEACON_GATE_H
#define _BEACON_GATE_H
#include <windows.h>

/* Beacon gate defines. */
#define MAX_BEACON_GATE_ARGUMENTS 10
#define beaconGate(i)	((BEACON_GATE_##i)gateFunction->functionPtr)
#define arg(i) (ULONG_PTR)gateFunction->args[i]

/* Enum to specify what WinAPI is being called. */
typedef enum _WinApi {
    INTERNETOPENA,
    INTERNETCONNECTA,
    VIRTUALALLOC,
    VIRTUALALLOCEX,
    VIRTUALPROTECT,
    VIRTUALPROTECTEX,
    VIRTUALFREE,
    GETTHREADCONTEXT,
    SETTHREADCONTEXT,
    RESUMETHREAD,
    CREATETHREAD,
    CREATEREMOTETHREAD,
    OPENPROCESS,
    OPENTHREAD,
    CLOSEHANDLE,
    CREATEFILEMAPPING,
    MAPVIEWOFFILE,
    UNMAPVIEWOFFILE,
    VIRTUALQUERY,
    DUPLICATEHANDLE,
    READPROCESSMEMORY,
    WRITEPROCESSMEMORY,
    EXITTHREAD,
} WinApi;

/**
 * FUNCTION_CALL struct which encapsulates atomic function call.
 *
 * functionPtr - target function to call
 * function - Enum representing target WinApi
 * numOfArgs - number of arguments
 * args - array of ULONG_PTRs containing the passed arguments (e.g. rcx, rdx, ...)
 * bMask - BOOL indicating whether Beacon should be masked during the call
 * ULONG_PTR - retValue of the atomic function call
 */
typedef struct {
    PVOID functionPtr;
    WinApi function;
    int numOfArgs;
    ULONG_PTR args[MAX_BEACON_GATE_ARGUMENTS];
    BOOL bMask;
    ULONG_PTR retValue;
} FUNCTION_CALL, * PFUNCTION_CALL;

/* Currently support max 10 arguments. */
/* NB For x86 we only support std call convention as this is what Windows uses for most Win32 APIs. */
typedef ULONG_PTR(__stdcall* BEACON_GATE_00)(VOID);
typedef ULONG_PTR(__stdcall* BEACON_GATE_01)(ULONG_PTR);
typedef ULONG_PTR(__stdcall* BEACON_GATE_02)(ULONG_PTR, ULONG_PTR);
typedef ULONG_PTR(__stdcall* BEACON_GATE_03)(ULONG_PTR, ULONG_PTR, ULONG_PTR);
typedef ULONG_PTR(__stdcall* BEACON_GATE_04)(ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR);
typedef ULONG_PTR(__stdcall* BEACON_GATE_05)(ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR);
typedef ULONG_PTR(__stdcall* BEACON_GATE_06)(ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR);
typedef ULONG_PTR(__stdcall* BEACON_GATE_07)(ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR);
typedef ULONG_PTR(__stdcall* BEACON_GATE_08)(ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR);
typedef ULONG_PTR(__stdcall* BEACON_GATE_09)(ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR);
typedef ULONG_PTR(__stdcall* BEACON_GATE_10)(ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR);
#endif // _BEACON_GATE_H
