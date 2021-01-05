//
// Created by Tobias on 1/5/2021.
//

#ifndef NPP_TESTWINDOW_HPP
#define NPP_TESTWINDOW_HPP

#include "Win32Context.hpp"

class TestWindow : public Win32Context<TestWindow> {
public:
	[[nodiscard]] LPCWSTR class_name() const override;
	LRESULT handle_message(UINT msg, WPARAM wp, LPARAM lp) override;
};

#endif // NPP_TESTWINDOW_HPP
