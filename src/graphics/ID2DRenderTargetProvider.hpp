//
// Created by Tobias on 1/18/2021.
//

#ifndef NPP_ID2DRENDERTARGETPROVIDER_HPP
#define NPP_ID2DRENDERTARGETPROVIDER_HPP

#include "../util/win_headers.hpp"
#include "../util/d2d_headers.hpp"

class ID2DRenderTargetProvider {
public:
	virtual HRESULT get_d2d_render_target(ID2D1Factory *, ID2D1RenderTarget **) = 0;
};

#endif // NPP_ID2DRENDERTARGETPROVIDER_HPP
