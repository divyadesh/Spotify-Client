#pragma once

#include "lib/json.hpp"
#include "lib/enum/albumsize.hpp"
#include "lib/enum/librarylayout.hpp"
#include "lib/enum/position.hpp"
#include "lib/enum/albumshape.hpp"

namespace lib
{
	namespace setting
	{
		/**
		 * Qt Widgets specific settings
		 */
		using qt = struct qt
		{
			/**
			 * Show system borders and title bar
			 */
			bool system_title_bar = false;

			/**
			 * Use a custom font instead of system default
			 */
			std::string custom_font_name;

			/**
			 * Use a custom font size instead of system default
			 * @note 0 if not set
			 */
			int custom_font_size = 0;

			/**
			 * Mirror buttons in title bar
			 * (requires system_title_bar to be false)
			 */
			bool mirror_title_bar
#ifdef __APPLE__
				= true;
#else
				= false;
#endif

			/**
			 * Size of album cover in side panel
			 */
			lib::album_size album_size = lib::album_size::fixed;

			/**
			 * How to display library/playlists
			 */
			lib::library_layout library_layout = lib::library_layout::stacked;

			/**
			 * Where to show the main toolbar
			 */
			lib::position toolbar_position = lib::position::top;

			/**
			 * Shape of album in main window and tray icon
			 */
            lib::album_shape album_shape = lib::album_shape::none;
		};

		void to_json(nlohmann::json &j, const qt &q);

		void from_json(const nlohmann::json &j, qt &q);
	}
}
