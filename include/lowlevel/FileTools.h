/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_FILE_TOOLS_H
#define SOLARUS_FILE_TOOLS_H

#include "Common.h"
#include <string>
#include <map>

struct lua_State;

/**
 * @brief Handles access to data files.
 *
 * This class provides some functions to open and analyze a data file.
 * It encapsulates how the access to data files is done.
 * This class is the only one that knows how to access a data file
 * (including the language-specific ones)
 * and is the only one that calls the PHYSFS library to get data files from
 * the data archive when necessary.
 */
class FileTools {

  public:

    // Initialization.
    static void initialize(int argc, char** argv);
    static void quit();

    // Reading data files of the quest.
    static bool data_file_exists(const std::string& file_name);
    static std::istream& data_file_open(const std::string& file_name,
        bool language_specific = false);
    static void data_file_close(const std::istream& data_file);
    static void data_file_open_buffer(const std::string& file_name,
        char** buffer, size_t* size, bool language_specific = false);
    static void data_file_save_buffer(const std::string& file_name,
        const char* buffer, size_t size);
    static void data_file_close_buffer(char* buffer);
    static void data_file_delete(const std::string& file_name);

    static void read(std::istream& is, int& value);
    static void read(std::istream& is, uint32_t& value);
    static void read(std::istream& is, std::string& value);

    // Writing files.
    static const std::string& get_solarus_write_dir();
    static void set_solarus_write_dir(const std::string& solarus_write_dir);
    static const std::string& get_quest_write_dir();
    static void set_quest_write_dir(const std::string& quest_write_dir);
    static const std::string get_full_quest_write_dir();
 
    // Languages.
    static bool has_language(const std::string& language_code);
    static void set_language(const std::string& language_code);
    static const std::string& get_language();
    static const std::string& get_default_language();
    static const std::map<std::string, std::string>& get_languages();

  private:

    static std::string get_base_write_dir();
    static void initialize_languages();
    static int l_language(lua_State* l);

    static std::string solarus_write_dir;                /**< Directory where the engine can write files, relative to the user's home. */
    static std::string quest_write_dir;                  /**< Write directory of the current quest, relative to solarus_write_dir. */

    static std::map<std::string, std::string> languages; /**< The languages available (code -> language name). */
    static std::string language_code;                    /**< Code of the current language (e.g. "en", "fr", etc.). */
    static std::string default_language_code;            /**< Code of the default language. */
};

#endif

