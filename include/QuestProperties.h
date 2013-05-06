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

#ifndef SOLARUS_QUEST_PROPERTIES_H
#define SOLARUS_QUEST_PROPERTIES_H

#include "Common.h"
#include <string>

struct lua_State;

/**
 * @brief This class reads the quest properties from file quest.lua
 * and applies them.
 *
 * These properties are general information about the quest. They include:
 * - the writing directory for savegames of this quest,
 * - the window title bar.
 *
 * The main reason why these quest properties are stored as a data file
 * (even if they are also available through the Solarus scripting API)
 * is to make them easily editable in quest editors.
 */
class QuestProperties {

  public:

    QuestProperties(MainLoop& main_loop);
    ~QuestProperties();

    void load();

  private:

    static int l_quest(lua_State* l);

    MainLoop& main_loop;  /**< The quest main loop. */
};

#endif

