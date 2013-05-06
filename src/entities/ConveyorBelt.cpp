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
#include "entities/ConveyorBelt.h"
#include "Map.h"
#include "Sprite.h"
#include "lowlevel/FileTools.h"

/**
 * @brief Creates a new conveyor belt.
 * @param name Unique name identifying the entity on the map or an empty string.
 * @param layer layer of the entity to create on the map
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param direction direction of the conveyor belt
 */
ConveyorBelt::ConveyorBelt(const std::string& name,
    Layer layer, int x, int y, int direction):
  Detector(COLLISION_RECTANGLE, name, layer, x, y, 16, 16) {

  set_origin(8, 13);
  create_sprite("entities/conveyor_belt");
  get_sprite().set_current_direction(direction);
  set_direction(direction);
}

/**
 * @brief Destructor.
 */
ConveyorBelt::~ConveyorBelt() {

}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType ConveyorBelt::get_type() {
  return CONVEYOR_BELT;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool ConveyorBelt::is_obstacle_for(MapEntity& other) {

  return other.is_conveyor_belt_obstacle(*this);
}

/**
 * @brief This function is called when another entity overlaps this entity.
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void ConveyorBelt::notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) {

  const Rectangle &xy_move = direction_to_xy_move(get_direction());
  entity_overlapping.notify_collision_with_conveyor_belt(*this, xy_move.get_x(), xy_move.get_y());
}

