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
#include "entities/Teletransporter.h"
#include "entities/Hero.h"
#include "Game.h"
#include "Sprite.h"
#include "Map.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Sound.h"

/**
 * @brief Constructor.
 * @param name Name of the teletransporter.
 * @param layer Layer of the teletransporter.
 * @param x X position of the teletransporter's rectangle.
 * @param y Y position of the teletransporter's rectangle.
 * @param width Width of the teletransporter's rectangle.
 * @param height Height of the teletransporter's rectangle.
 * @param sprite_name Sprite animation set id to use, or an empty string.
 * @param sound_id Sound to play when using the teletransporter,
 * or an empty string.
 * @param transition_style Style of transition between the two maps.
 * @param destination_map_id Id of the destination map.
 * @param destination_name Location on the destination map,
 * or "_same" to keep the hero's coordinates,
 * or "_side" to place the hero on the appropriate side of the map.
 */
Teletransporter::Teletransporter(
    const std::string& name,
    Layer layer,
    int x,
    int y,
    int width,
    int height,
    const std::string& sprite_name,
    const std::string& sound_id,
    Transition::Style transition_style,
    const std::string& destination_map_id,
    const std::string& destination_name):

  Detector(COLLISION_CUSTOM, name, layer, x, y, width, height),
  sound_id(sound_id),
  transition_style(transition_style),
  destination_map_id(destination_map_id),
  destination_name(destination_name),
  destination_side(-1),
  transporting_hero(false) {
  
  if (!sprite_name.empty()) {
    create_sprite(sprite_name);
  }
}

/**
 * @brief Destructor.
 */
Teletransporter::~Teletransporter() {
}

/**
 * @brief Sets the map.
 *
 * Warning: as this function is called when initializing the map,
 * the current map of the game is still the old one.
 *
 * @param map the map
 */
void Teletransporter::set_map(Map& map) {

  MapEntity::set_map(map);

  if (destination_name == "_side") {

    int x = get_x();
    int y = get_y();

    if (get_width() == 16 && x == -16) {
      destination_side = 0;
    }
    else if (get_width() == 16 && x == map.get_width()) {
      destination_side = 2;
    }
    else if (get_height() == 16 && y == -16) {
      destination_side = 3;
    }
    else if (get_height() == 16 && y == map.get_height()) {
      destination_side = 1;
    }
    else {
      Debug::die(StringConcat() << "Bad position of teletransporter '" << get_name() << "'"); 
    }
  }

  transition_direction = (destination_side + 2) % 4;
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Teletransporter::get_type() {
  return TELETRANSPORTER;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Teletransporter::is_obstacle_for(MapEntity& other) {

  return other.is_teletransporter_obstacle(*this);
}

/**
 * @brief Returns whether an entity's collides with this entity.
 * @param entity an entity
 * @return true if the entity's collides with this entity
 */
bool Teletransporter::test_collision_custom(MapEntity& entity) {

  bool collision = false;
  bool normal_case = true;

  // specific collision tests for some situations
  if (entity.is_hero()) {

    Hero& hero = (Hero&) entity;
    if (is_on_map_side()) {
      // scrolling towards an adjacent map
      Rectangle facing_point = hero.get_facing_point(transition_direction);
      collision = hero.is_moving_towards(transition_direction)
	    && overlaps(facing_point.get_x(), facing_point.get_y());
      normal_case = false;
    }

    else if (!get_map().test_collision_with_border(get_center_point()) &&
        hero.get_ground() == GROUND_HOLE) {
      // falling into a hole
      collision = overlaps(hero.get_ground_point());
      normal_case = false;
    }
  }

  // normal case
  if (normal_case) {
    const Rectangle& entity_rectangle = entity.get_bounding_box();
    int x1 = entity_rectangle.get_x() + 4;
    int x2 = x1 + entity_rectangle.get_width() - 9;
    int y1 = entity_rectangle.get_y() + 4;
    int y2 = y1 + entity_rectangle.get_height() - 9;

    collision = overlaps(x1, y1) && overlaps(x2, y1) &&
      overlaps(x1, y2) && overlaps(x2, y2);
  }

  if (!collision && !is_on_map_side()) {
    transporting_hero = false;
  }

  return collision;
}

/**
 * @brief This function is called by the engine when an entity overlaps the teletransporter.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Teletransporter::notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) {

  entity_overlapping.notify_collision_with_teletransporter(*this, collision_mode);
}

/**
 * @brief Makes the teletransporter move the hero to the destination.
 * @param hero the hero
 */
void Teletransporter::transport_hero(Hero& hero) {

  if (transporting_hero) {
    // already done
    return;
  }
  transporting_hero = true;

  if (!sound_id.empty()) {
    Sound::play(sound_id);
  }

  std::string name = destination_name;
  int hero_x = hero.get_x();
  int hero_y = hero.get_y();

  if (is_on_map_side()) {

    // special destination point: side of the map
    // we determine the appropriate side based on the teletransporter's position;
    // we also place the hero on the old map so that its position corresponds to the new map

    switch (destination_side) {

    case 0:
      name += '0'; // scroll to the west
      hero_x = 0;
      break;

    case 1:
      name += '1'; // scroll to the south
      hero_y = get_map().get_height() + 5;
      break;

    case 2:
      name += '2'; // scroll to the east
      hero_x = get_map().get_width();
      break;

    case 3:
      name += '3'; // scroll to the north
      hero_y = 5;
      break;

    default:
      Debug::die(StringConcat() << "Bad destination side for teletransporter '" << get_name() << "'"); 
    }
  }

  get_game().set_current_map(destination_map_id, name, transition_style);
  hero.set_xy(hero_x, hero_y);
}

/**
 * @brief Returns whether this teletransporter is on the side of the map.
 *
 * When true is returned, this means that the teletransporter can make the hero
 * scroll towards an adjacent map.
 *
 * @return true if this teletransporter is on the side of the map
 */
bool Teletransporter::is_on_map_side() {
  return destination_side >= 0;
}

