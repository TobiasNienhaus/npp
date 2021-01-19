//
// Created by Tobias on 1/19/2021.
//

#ifndef NPP_GLOBALS_HPP
#define NPP_GLOBALS_HPP

class Globals {
	// Singleton stuff
private:
	Globals() = default;

public:
	Globals(const Globals &) = delete;
	Globals(Globals &&) = delete;
	Globals &operator=(const Globals &) = delete;
	Globals &operator=(Globals &&) = delete;

public:
	~Globals() = default;
	static Globals &instance();

private:
	bool m_clearDrawingSurface{};
	bool m_redrawDrawingSurface{};

public:
	static bool &clear_drawing_surface();
	static bool &redraw_drawing_surface();

};

#endif // NPP_GLOBALS_HPP
