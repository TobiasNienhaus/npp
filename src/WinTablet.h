//
// Created by Tobias on 12/9/2020.
//

#ifndef NPP_WINTABLET_H
#define NPP_WINTABLET_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "wacom_def.h"

class WinTablet {
public:
	struct Status {
		bool enable = false;
		int x = 0;
		int y = 0;
		int pressure = 0;
		bool button = false;
	};

	WinTablet() = default;

	~WinTablet();

	bool app_init(const char *dllPath = "Wintab32.dll");

	void app_cleanup();

	void init(HWND hwnd);

	void cleanup();

	void on_wm_activate(HWND hwnd, WPARAM wparam, LPARAM lparam);

	Status on_wt_packet(HWND hwnd, WPARAM wparam, LPARAM lparam);

	bool m_wintabInitialized = false;
	LOGCONTEXT m_logContext = {0};
	HCTX m_hctx = nullptr;

protected:
	bool load_dll(const char *path);

	bool get_proc_address();

	void unload_dll();

	typedef UINT (API * WTINFOA)(UINT, UINT, LPVOID);
	typedef HCTX (API * WTOPENA)(HWND, LPLOGCONTEXTA, BOOL);
	typedef BOOL (API * WTGETA)(HCTX, LPLOGCONTEXT);
	typedef BOOL (API * WTSETA)(HCTX, LPLOGCONTEXT);
	typedef BOOL (API * WTCLOSE)(HCTX);
	typedef BOOL (API * WTENABLE)(HCTX, BOOL);
	typedef BOOL (API * WTPACKET)(HCTX, UINT, LPVOID);
	typedef BOOL (API * WTOVERLAP)(HCTX, BOOL);
	typedef BOOL (API * WTSAVE)(HCTX, LPVOID);
	typedef BOOL (API * WTCONFIG)(HCTX, HWND);
	typedef HCTX (API * WTRESTORE)(HWND, LPVOID, BOOL);
	typedef BOOL (API * WTEXTSET)(HCTX, UINT, LPVOID);
	typedef BOOL (API * WTEXTGET)(HCTX, UINT, LPVOID);
	typedef BOOL (API * WTQUEUESIZESET)(HCTX, int);
	typedef int  (API * WTDATAPEEK)(HCTX, UINT, UINT, int, LPVOID, LPINT);
	typedef int  (API * WTPACKETSGET)(HCTX, int, LPVOID);
	typedef HMGR (API * WTMGROPEN)(HWND, UINT);
	typedef BOOL (API * WTMGRCLOSE)(HMGR);
	typedef HCTX (API * WTMGRDEFCONTEXT)(HMGR, BOOL);
	typedef HCTX (API * WTMGRDEFCONTEXTEX)(HMGR, UINT, BOOL);

	WTINFOA gpWTInfoA = nullptr;
	WTOPENA gpWTOpenA = nullptr;
	WTGETA gpWTGetA = nullptr;
	WTSETA gpWTSetA = nullptr;
	WTCLOSE gpWTClose = nullptr;
	WTPACKET gpWTPacket = nullptr;
	WTENABLE gpWTEnable = nullptr;
	WTOVERLAP gpWTOverlap = nullptr;
	WTSAVE gpWTSave = nullptr;
	WTCONFIG gpWTConfig = nullptr;
	WTRESTORE gpWTRestore = nullptr;
	WTEXTSET gpWTExtSet = nullptr;
	WTEXTGET gpWTExtGet = nullptr;
	WTQUEUESIZESET gpWTQueueSizeSet = nullptr;
	WTDATAPEEK gpWTDataPeek = nullptr;
	WTPACKETSGET gpWTPacketsGet = nullptr;
	WTMGROPEN gpWTMgrOpen = nullptr;
	WTMGRCLOSE gpWTMgrClose = nullptr;
	WTMGRDEFCONTEXT gpWTMgrDefContext = nullptr;
	WTMGRDEFCONTEXTEX gpWTMgrDefContextEx = nullptr;

	HINSTANCE m_ghWintab = nullptr;
};

#endif // NPP_WINTABLET_H
