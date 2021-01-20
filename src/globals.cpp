//
// Created by Tobias on 1/19/2021.
//

#include "globals.hpp"
#include <iostream>

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
std::optional<std::string> &Globals::filename() {
	return instance().m_filename;
}
bool &Globals::has_unsaved_changes() {
	return instance().m_unsavedChanges;
}
std::optional<std::string> &Globals::file_to_load() {
	return instance().m_fileToLoad;
}

Globals::callback_id_t Globals::new_id() {
	static Globals::callback_id_t id{};
	return id++;
}

Globals::callback_id_t Globals::register_save_callback(const Globals::callback_t& cb) {
	if(cb) {
		auto id{new_id()};
		instance().m_saveCallbacks.emplace(id, cb);
		return id;
	}
	return -1;
}

bool Globals::deregister_save_callback(Globals::callback_id_t id) {
	if(instance().m_saveCallbacks.contains(id)) {
		instance().m_saveCallbacks.erase(id);
		return true;
	}
	return false;
}

Globals::callback_id_t Globals::register_open_callback(const Globals::callback_t& cb) {
	if(cb) {
		auto id{new_id()};
		instance().m_openCallbacks.emplace(id, cb);
		return id;
	}
	return -1;
}

bool Globals::deregister_open_callback(Globals::callback_id_t id) {
	if(instance().m_openCallbacks.contains(id)) {
		instance().m_openCallbacks.erase(id);
		return true;
	}
	return false;
}

void Globals::on_save_file() {
	for(const auto &[_, cb] : instance().m_saveCallbacks) {
		cb();
	}
}

void Globals::on_open_file() {
	for(const auto &[_, cb] : instance().m_openCallbacks) {
		cb();
	}
}
