//
// Created by Tobias on 1/12/2021.
//

#ifndef NPP_IMGUIHANDLER_HPP
#define NPP_IMGUIHANDLER_HPP

#include "../../util/win_headers.hpp"
#include "../../util/d3d10_headers.hpp"

class ImGuiHandler {
public:
	ImGuiHandler(HWND window, ID3D10Device *device, ID3D10RenderTargetView *renderTarget);
	virtual ~ImGuiHandler();

	void frame_prep();
	void frame();
	LRESULT handle_message(UINT msg, WPARAM wp, LPARAM lp);
	void set_render_target(ID3D10RenderTargetView *newTarget);

protected:
	virtual void draw();

private:
	HWND m_window;
	ID3D10Device *m_device;
	ID3D10RenderTargetView *m_renderTarget;
};

#endif // NPP_IMGUIHANDLER_HPP
