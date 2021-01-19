//
// Created by Tobias on 1/19/2021.
//

#include "globals.hpp"

Globals &Globals::instance() {
	static auto *globals = new Globals();
	return *globals;
}

bool &Globals::clear_drawing_surface() {
	return instance().m_clearDrawingSurface;
}

bool &Globals::redraw_drawing_surface() {
	return instance().m_redrawDrawingSurface;
}
