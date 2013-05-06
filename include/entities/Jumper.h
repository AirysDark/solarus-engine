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
#ifndef SOLARUS_JUMPER_H
#define SOLARUS_JUMPER_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * @brief A sensor that makes the hero jump.
 *
 * A jumper is a detector placed on a map.
 * When the hero touches this detetor, he makes a jump with a specified length.
 */
class Jumper: public Detector {

  private:

    int jump_length; /**< length of the jump in pixels (should be a multiple of 8) */

  public:

    Jumper(const std::string &name, Layer layer, int x, int y, int width, int height,
	int direction, int jump_length);
    ~Jumper();

    EntityType get_type();
    bool can_be_drawn();

    bool is_obstacle_for(MapEntity &other);
    bool is_point_in_diagonal(const Rectangle &point);
    bool test_collision_custom(MapEntity &entity);
    void notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode);
    int get_jump_length();

};

#endif

