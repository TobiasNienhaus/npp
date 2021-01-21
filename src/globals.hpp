//
// Created by Tobias on 1/19/2021.
//

#ifndef NPP_GLOBALS_HPP
#define NPP_GLOBALS_HPP

#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <string>

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

	using callback_id_t = unsigned long long int;
	using callback_t = std::function<void()>;

private:
	static callback_id_t new_id();
	std::map<callback_id_t, callback_t> m_saveCallbacks{};
	std::map<callback_id_t, callback_t> m_openCallbacks{};

	bool m_clearDrawingSurface{};
	bool m_redrawDrawingSurface{};
	bool m_showDialog{};
	std::optional<std::string> m_filename{};
	std::optional<std::string> m_fileToLoad{};
	bool m_unsavedChanges{};

	bool m_wantsToClose{};

public:
	static bool &clear_drawing_surface();
	static bool &redraw_drawing_surface();
//	static bool &show_dialog();
	static std::optional<std::string> &filename();
	static std::optional<std::string> &file_to_load();
	static bool &has_unsaved_changes();

	static callback_id_t register_save_callback(const callback_t& cb);
	static bool deregister_save_callback(callback_id_t id);
	static callback_id_t register_open_callback(const callback_t& cb);
	static bool deregister_open_callback(callback_id_t id);

	static void on_save_file();
	static void on_open_file();

	static bool &wants_to_close();

};

#endif // NPP_GLOBALS_HPP
