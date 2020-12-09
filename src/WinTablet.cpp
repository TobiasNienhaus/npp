//
// Created by Tobias on 12/9/2020.
//

#include "WinTablet.h"
#include <assert.h>
#include <stdio.h>

#define WACOM_ASSERT(...) assert(__VA_ARGS__)

static void WacomTrace(const char *lpszFormat, ...) {
	char szTraceMessage[128];
	int nBytesWritten;
	va_list args;
	WACOM_ASSERT(lpszFormat);
	va_start(args, lpszFormat);
	nBytesWritten = vsprintf_s(szTraceMessage, sizeof(szTraceMessage) - 1,
							   lpszFormat, args);
	if (nBytesWritten > 0) { OutputDebugStringA(szTraceMessage); }
	va_end(args);
}

WinTablet::~WinTablet() {
	app_cleanup();
}

bool WinTablet::app_init(const char *dllPath) {
	if (load_dll(dllPath)) { m_wintabInitialized = gpWTInfoA(0, 0, nullptr); }
	return m_wintabInitialized;
}

void WinTablet::app_cleanup() {
	cleanup();
	unload_dll();
	m_wintabInitialized = false;
}

void WinTablet::init(HWND hwnd) {
	if (!m_wintabInitialized) { return; }
	if (m_hctx) { return; }

	UINT wWTInfoRetVal = 0;

	m_logContext.lcOptions |= CXO_SYSTEM;

	wWTInfoRetVal = gpWTInfoA(WTI_DEFSYSCTX, 0, &m_logContext);
	WACOM_ASSERT(wWTInfoRetVal == sizeof(LOGCONTEXT));

	WACOM_ASSERT(m_logContext.lcOptions & CXO_SYSTEM);

	wsprintf(m_logContext.lcName, "PrsTest Digitizing");

	m_logContext.lcOptions |= CXO_MESSAGES;

	m_logContext.lcPktData = PACKETDATA;

	m_logContext.lcPktMode = PACKETMODE;

	m_logContext.lcMoveMask = PACKETDATA;

	m_logContext.lcBtnUpMask = m_logContext.lcBtnDnMask;

	AXIS tabletX = {0};
	wWTInfoRetVal = gpWTInfoA(WTI_DEVICES + 0, DVC_X, &tabletX);
	WACOM_ASSERT(wWTInfoRetVal == sizeof(AXIS));

	AXIS tabletY = {0};
	wWTInfoRetVal = gpWTInfoA(WTI_DEVICES + 0, DVC_Y, &tabletY);
	WACOM_ASSERT(wWTInfoRetVal == sizeof(AXIS));

	// Set origin of tablet axes (0, 0)
	m_logContext.lcInOrgX = 0;
	m_logContext.lcInOrgY = 0;
	// Set max of tablet axes (x-max, y-max)
	m_logContext.lcInExtX = tabletX.axMax;
	m_logContext.lcInExtY = tabletY.axMax;

	m_logContext.lcOutOrgX = GetSystemMetrics(SM_XVIRTUALSCREEN);
	m_logContext.lcOutOrgY = GetSystemMetrics(SM_YVIRTUALSCREEN);
	m_logContext.lcOutExtX = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	// Tablet has flipped origin
	m_logContext.lcOutExtY = -GetSystemMetrics(SM_CYVIRTUALSCREEN);

	HCTX hctx = gpWTOpenA(hwnd, &m_logContext, FALSE);

	WacomTrace("HCTX: %i\n", hctx);

	m_hctx = hctx;
}

void WinTablet::cleanup() {
	if (m_hctx) {
		gpWTClose(m_hctx);
		m_hctx = nullptr;
	}
}

void WinTablet::on_wm_activate(HWND, WPARAM wparam, LPARAM) {
	if (!m_hctx) { return; }
	const auto fActive = LOWORD(wparam);
	gpWTEnable(m_hctx, fActive);
	if (fActive) { gpWTOverlap(m_hctx, TRUE); }
}

WinTablet::Status WinTablet::on_wt_packet(HWND, WPARAM wparam, LPARAM lparam) {
	if (!m_hctx) { return {}; }
	PACKET packet;
	if (!gpWTPacket(reinterpret_cast<HCTX>(lparam), static_cast<UINT>(wparam),
					&packet)) {
		return {};
	}
	return Status{.enable = true,
				  .x = packet.pkX,
				  .y = packet.pkY,
				  .pressure = static_cast<int>(packet.pkNormalPressure),
				  .button = (HIWORD(packet.pkButtons) == TBN_DOWN)};
}

bool WinTablet::load_dll(const char *path) {
	unload_dll();
	m_ghWintab = LoadLibraryA(path);
	if (!m_ghWintab) {
		return false;
	} else {
		return get_proc_address();
	}
}

bool WinTablet::get_proc_address() {
#pragma push_macro("GETPROCADDRESS")
#define GETPROCADDRESS(type, func) \
		gp##func = (type)GetProcAddress(m_ghWintab, #func);\
		if(!gp##func) { WACOM_ASSERT(FALSE); unload_dll(); return false; }

	GETPROCADDRESS(WTOPENA, WTOpenA)
	GETPROCADDRESS(WTINFOA, WTInfoA)
	GETPROCADDRESS(WTGETA, WTGetA)
	GETPROCADDRESS(WTSETA, WTSetA)
	GETPROCADDRESS(WTPACKET, WTPacket)
	GETPROCADDRESS(WTCLOSE, WTClose)
	GETPROCADDRESS(WTENABLE, WTEnable)
	GETPROCADDRESS(WTOVERLAP, WTOverlap)
	GETPROCADDRESS(WTSAVE, WTSave)
	GETPROCADDRESS(WTCONFIG, WTConfig)
	GETPROCADDRESS(WTRESTORE, WTRestore)
	GETPROCADDRESS(WTEXTSET, WTExtSet)
	GETPROCADDRESS(WTEXTGET, WTExtGet)
	GETPROCADDRESS(WTQUEUESIZESET, WTQueueSizeSet)
	GETPROCADDRESS(WTDATAPEEK, WTDataPeek)
	GETPROCADDRESS(WTPACKETSGET, WTPacketsGet)
	GETPROCADDRESS(WTMGROPEN, WTMgrOpen)
	GETPROCADDRESS(WTMGRCLOSE, WTMgrClose)
	GETPROCADDRESS(WTMGRDEFCONTEXT, WTMgrDefContext)
	GETPROCADDRESS(WTMGRDEFCONTEXTEX, WTMgrDefContextEx)

#pragma pop_macro("GETPROCADDRESS")
	return true;
}

void WinTablet::unload_dll() {
	if(m_ghWintab) {
		FreeLibrary(m_ghWintab);
		m_ghWintab = nullptr;
	}
}
