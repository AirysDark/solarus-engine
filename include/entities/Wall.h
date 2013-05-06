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
#ifndef SOLARUS_WALL_H
#define SOLARUS_WALL_H

#include "Common.h"
#include "entities/MapEntity.h"
#include <map>

/**
 * @brief An invisible obstacle that stops other types of entities.
 *
 * The types of entities that are stopped can be specified.
 * This entity is typically used to prevent enemies from going to
 * a specific place.
 */
class Wall: public MapEntity {

  private:

    std::map<EntityType, bool> entity_types_stopped;

    bool enabled;              /**< indicates that the obstacle is enabled (default: yes) */
    bool waiting_enabled;      /**< indicates that the obstacle is waiting to be enabled */

  public:

    Wall(const std::string &name, Layer layer, int x, int y, int width, int height,
        bool stops_hero, bool stops_enemies, bool stops_npcs, bool stops_blocks);
    ~Wall();

    EntityType get_type();
    bool can_be_drawn();
    bool is_obstacle_for(MapEntity &other);
};

#endif

