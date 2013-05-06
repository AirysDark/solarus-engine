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
#ifndef SOLARUS_DYNAMIC_TILE_H
#define SOLARUS_DYNAMIC_TILE_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * @brief A special tile that can be enabled or disabled.
 *
 * A dynamic tile is a tile placed on the map 
 * that can be enabled or disabled by the script,
 * contrary to the Tile instances that are totally static for performance reasons.
 * An enabled dynamic tile behaves like a normal tile and may be an obstacle.
 * A disabled dynamic tile is invisible and can be traversed.
 */
class DynamicTile: public Detector {

  private:

    int tile_pattern_id;       /**< id of the tile pattern */
    TilePattern *tile_pattern; /**< pattern of the tile */

  protected:

    bool test_collision_custom(MapEntity &entity);
    void notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode);

  public:

    DynamicTile(const std::string &name, Layer layer, int x, int y,
	int width, int height, int tile_pattern_id, bool visible);
    ~DynamicTile();

    EntityType get_type();
    void set_map(Map &map);
    bool is_obstacle_for(MapEntity &other);
    void draw_on_map();
    void notify_enabled(bool enabled);
};

#endif

