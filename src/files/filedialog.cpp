//
// Created by Tobias on 1/20/2021.
//

#include "filedialog.hpp"

#include "../globals.hpp"

static auto *det_dialog() {
	return ImGuiFileDialog::Instance();
}

npp::file::dialog::Status &npp::file::dialog::status() {
	static auto st{Status::NONE};
	return st;
}

void npp::file::dialog::open(Status openMode) {
	static std::string titleOpen{"Open File"};
	static std::string titleSave{"Save As"};
	constexpr const char *filters =
		"Plain (*.npp){.npp},Binary (*.bnpp){.bnpp}";
	static std::string path = "F:/Dokumente"; // TODO replace with WinAPI one

	status() = openMode;

	ImGuiFileDialogFlags flags = ImGuiFileDialogFlags_None;
	if (status() != Status::OPEN) {
		flags |= ImGuiFileDialogFlags_ConfirmOverwrite;
	}

	det_dialog()->OpenModal(
		"DIALOG", openMode == Status::OPEN ? titleOpen : titleSave, filters,
		Globals::filename().value_or(path), 1, nullptr, flags);
}

void npp::file::dialog::display() {
	bool reopen = false;
	if (det_dialog()->Display("DIALOG")) {
		if (det_dialog()->IsOk()) {
			switch (status()) {
			case Status::SAVE_AS:
				Globals::filename() = det_dialog()->GetFilePathName();
				Globals::on_save_file();
				status() = Status::NONE;
				break;
			case Status::OPEN:
				Globals::file_to_load() = det_dialog()->GetFilePathName();
				Globals::on_open_file();
				status() = Status::NONE;
				break;
			case Status::SAVE_OPEN:
				Globals::filename() = det_dialog()->GetFilePathName();
				Globals::on_save_file();
				reopen = true;
				break;
			case Status::SAVE_AS_THEN_CLEAR:
				Globals::filename() = det_dialog()->GetFilePathName();
				Globals::on_save_file();
				Globals::file_to_load() = {};
				Globals::on_open_file();
				status() = Status::NONE;
				break;
			case Status::NONE:
			default:
				break;
			}
		}
		det_dialog()->Close();
	}
	if (reopen) {
		status() = Status::OPEN;
		open(Status::OPEN);
	}
}
