//
// Created by Tobias on 1/20/2021.
//

#ifndef NPP_FILEDIALOG_HPP
#define NPP_FILEDIALOG_HPP

#include <ImGuiFileDialog.h>

namespace npp::file::dialog {

enum class Status { NONE = -1, SAVE_AS, OPEN, SAVE_OPEN, SAVE_AS_THEN_CLEAR };

Status &status();

void open(Status openMode);

void display();

} // namespace npp::file::dialog

#endif // NPP_FILEDIALOG_HPP
