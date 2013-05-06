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
#include "lua/LuaContext.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>

const std::string LuaContext::input_module_name = "sol.input";

/**
 * @brief Initializes the input features provided to Lua.
 */
void LuaContext::register_input_module() {

  static const luaL_Reg functions[] = {
      // no function from Lua to C++ for inputs (yet)
      { NULL, NULL }
  };
  // create the "sol.input" table anyway
  register_functions(input_module_name, functions);
}

