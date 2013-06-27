/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/MapEntities.h"
#include "entities/Hero.h"
#include "entities/Tile.h"
#include "entities/TilePattern.h"
#include "entities/Layer.h"
#include "entities/Obstacle.h"
#include "entities/CrystalBlock.h"
#include "entities/Boomerang.h"
#include "entities/Stairs.h"
#include "entities/CameraStopper.h"
#include "Map.h"
#include "Game.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Color.h"
#include "lowlevel/Music.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
using std::list;

/**
 * \brief Constructor.
 * \param game the game
 * \param map the map (not loaded yet)
 */
MapEntities::MapEntities(Game& game, Map& map):
  game(game),
  map(map),
  hero(game.get_hero()),
  music_before_miniboss(Music::none) {

  Layer layer = hero.get_layer();
  this->obstacle_entities[layer].push_back(&hero);
  this->entities_drawn_y_order[layer].push_back(&hero);
  // TODO update that when the layer changes, same thing for enemies
  this->named_entities[hero.get_name()] = &hero;

  // surfaces to pre-render static tiles
  for (int layer = 0; layer < LAYER_NB; layer++) {
    non_animated_tiles_surfaces[layer] = NULL;
  }
}

/**
 * \brief Destructor.
 */
MapEntities::~MapEntities() {

  destroy_all_entities();
}

/**
 * \brief Removes all entities from the map.
 *
 * This function is called by the destructor or when the map is unloaded.
 */
void MapEntities::destroy_all_entities() {

  // delete tiles and clear lists sorted by layer
  for (int layer = 0; layer < LAYER_NB; layer++) {

    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      destroy_entity(tiles[layer][i]);
    }

    tiles[layer].clear();
    delete[] obstacle_tiles[layer];
    delete[] animated_tiles[layer];
    delete non_animated_tiles_surfaces[layer];

    entities_drawn_first[layer].clear();
    entities_drawn_y_order[layer].clear();
    obstacle_entities[layer].clear();
    stairs[layer].clear();
  }

  // delete the other entities

  list<MapEntity*>::iterator i;
  for (i = all_entities.begin(); i != all_entities.end(); i++) {
    destroy_entity(*i);
  }
  all_entities.clear();
  named_entities.clear();

  detectors.clear();
  entities_to_remove.clear();
}

/**
 * \brief Destroys an entity.
 *
 * The object is freed if it is not used anywhere else.
 *
 * \param entity The entity to destroy.
 */
void MapEntities::destroy_entity(MapEntity* entity) {

  if (!entity->is_being_removed()) {
    entity->notify_being_removed();
  }

  entity->decrement_refcount();
  if (entity->get_refcount() == 0) {
    delete entity;
  }
}

/**
 * \brief Returns the hero.
 * \return the hero
 */
Hero& MapEntities::get_hero() {
  return hero;
}

/**
 * \brief Returns the entities (other that tiles) such that the hero cannot walk on them.
 * \param layer the layer
 * \return the obstacle entities on that layer
 */
const list<MapEntity*>& MapEntities::get_obstacle_entities(Layer layer) {
  return obstacle_entities[layer];
}

/**
 * \brief Returns all detectors on the map.
 * \return the detectors
 */
const list<Detector*>& MapEntities::get_detectors() {
  return detectors;
}

/**
 * \brief Returns all stairs on the specified layer.
 * \param layer the layer
 * \return the stairs on this layer
 */
const list<Stairs*>& MapEntities::get_stairs(Layer layer) {
  return stairs[layer];
}

/**
 * \brief Returns all crystal blocks on the specified layer.
 * \param layer the layer
 * \return the crystal blocks on this layer
 */
const list<CrystalBlock*>& MapEntities::get_crystal_blocks(Layer layer) {
  return crystal_blocks[layer];
}

/**
 * \brief Returns all camera stoppers of the map..
 * \return The camera stoppers.
 */
const list<CameraStopper*>& MapEntities::get_camera_stoppers() {
  return camera_stoppers;
}

/**
 * \brief Sets the obstacle tile property of an 8*8 square of the map.
 * \param layer layer of the square
 * \param x8 x coordinate of the square (divided by 8)
 * \param y8 y coordinate of the square (divided by 8)
 * \param obstacle the obstacle property to set
 */
void MapEntities::set_obstacle(int layer, int x8, int y8, Obstacle obstacle) {

  if (x8 >= 0 && x8 < map_width8 && y8 >= 0 && y8 < map_height8) {
    int index = y8 * map_width8 + x8;
    obstacle_tiles[layer][index] = obstacle;
  }
}

/**
 * \brief Returns the entity with the specified name.
 *
 * The program stops if there is no such entity.
 *
 * \param name Name of the entity to get.
 * \return The entity requested.
 */
MapEntity* MapEntities::get_entity(const std::string& name) {

  MapEntity* entity = find_entity(name);

  Debug::check_assertion(entity != NULL, StringConcat()
      << "Map '" << map.get_id()
      << "': Cannot find entity with name '" << name << "'");

  return entity;
}

/**
 * \brief Returns the entity with the specified name, or NULL if it doesn't exist.
 * \param name Name of the entity to find.
 * \return The entity requested, or NULL if there is no entity with the specified name.
 */
MapEntity* MapEntities::find_entity(const std::string& name) {

  if (named_entities.find(name) == named_entities.end()) {
    return NULL;
  }

  MapEntity* entity = named_entities[name];

  if (entity->is_being_removed()) {
    return NULL;
  }

  return entity;
}

/**
 * \brief Returns the entities of the map having the specified name prefix.
 * \param prefix Prefix of the name.
 * \return The entities of this type and having this prefix in their name.
 */
list<MapEntity*> MapEntities::get_entities_with_prefix(const std::string& prefix) {

  list<MapEntity*> entities;

  list<MapEntity*>::iterator i;
  for (i = all_entities.begin(); i != all_entities.end(); i++) {

    MapEntity* entity = *i;
    if (entity->has_prefix(prefix) && !entity->is_being_removed()) {
      entities.push_back(entity);
    }
  }

  return entities;
}

/**
 * \brief Returns the entities of the map with the specified type and having
 * the specified name prefix.
 * \param type Type of entity.
 * \param prefix Prefix of the name.
 * \return The entities of this type and having this prefix in their name.
 */
list<MapEntity*> MapEntities::get_entities_with_prefix(
    EntityType type, const std::string& prefix) {

  list<MapEntity*> entities;

  list<MapEntity*>::iterator i;
  for (i = all_entities.begin(); i != all_entities.end(); i++) {

    MapEntity* entity = *i;
    if (entity->get_type() == type && entity->has_prefix(prefix) && !entity->is_being_removed()) {
      entities.push_back(entity);
    }
  }

  return entities;
}

/**
 * \brief Returns whether there exists at least one entity with the specified
 * name prefix on the map.
 * \param prefix Prefix of the name.
 * \return \c true if there exists an entity with this prefix.
 */
bool MapEntities::has_entity_with_prefix(const std::string& prefix) {

  list<MapEntity*>::iterator i;
  for (i = all_entities.begin(); i != all_entities.end(); i++) {

    MapEntity* entity = *i;
    if (entity->has_prefix(prefix) && !entity->is_being_removed()) {
      return true;
    }
  }

  return false;
}

/**
 * \brief Brings to front an entity that is displayed as a sprite in the normal order.
 * \param entity the entity to bring to front
 */
void MapEntities::bring_to_front(MapEntity *entity) {

  Debug::check_assertion(entity->can_be_drawn(),
      StringConcat() << "Cannot bring to front entity '" << entity->get_name() << "' since it is not drawn");

  Debug::check_assertion(!entity->is_drawn_in_y_order(),
    StringConcat() << "Cannot bring to front entity '" << entity->get_name() << "' since it is drawn in the y order");

  Layer layer = entity->get_layer();
  entities_drawn_first[layer].remove(entity);
  entities_drawn_first[layer].push_back(entity);
}

/**
 * \brief Notifies all entities of the map that the map has just become active.
 */
void MapEntities::notify_map_started() {

  list<MapEntity*>::iterator i;
  for (i = all_entities.begin(); i != all_entities.end(); i++) {
    MapEntity *entity = *i;
    entity->notify_map_started();
    entity->notify_tileset_changed();
  }
  hero.notify_map_started();
  hero.notify_tileset_changed();

  // pre-render non-animated tiles
  build_non_animated_tiles();
}

/**
 * \brief Notifies all entities that the opening transition
 * of the map is finished.
 */
void MapEntities::notify_map_opening_transition_finished() {

  list<MapEntity*>::iterator i;
  for (i = all_entities.begin(); i != all_entities.end(); i++) {
    MapEntity* entity = *i;
    entity->notify_map_opening_transition_finished();
  }
  hero.notify_map_opening_transition_finished();
}

/**
 * \brief Notifies this entity manager that the tileset of the map has
 * changed.
 */
void MapEntities::notify_tileset_changed() {

  // Redraw optimized tiles (i.e. non animated ones).
  redraw_non_animated_tiles();

  list<MapEntity*>::iterator i;
  for (i = all_entities.begin(); i != all_entities.end(); i++) {
    MapEntity* entity = *i;
    entity->notify_tileset_changed();
  }
  hero.notify_map_opening_transition_finished();
}

/**
 * \brief Adds a tile on the map.
 *
 * This function is called for each tile when loading the map.
 * The tiles cannot change during the game.
 *
 * \param tile the tile to add
 */
void MapEntities::add_tile(Tile *tile) {

  Layer layer = tile->get_layer();

  // add the tile to the map
  tiles[layer].push_back(tile);
  tile->set_map(map);

  // update the collision list
  Obstacle obstacle = tile->get_tile_pattern().get_obstacle();

  int tile_x8 = tile->get_x() / 8;
  int tile_y8 = tile->get_y() / 8;
  int tile_width8 = tile->get_width() / 8;
  int tile_height8 = tile->get_height() / 8;

  int i, j;
  Obstacle non_obstacle_triangle;
 
  switch (obstacle) {

    /* If the obstacle property is the same for all points inside the base tile,
     * then all 8*8 squares of the extended tile have the same property.
     */
  case OBSTACLE_NONE:
  case OBSTACLE_SHALLOW_WATER:
  case OBSTACLE_DEEP_WATER:
  case OBSTACLE_HOLE:
  case OBSTACLE_LAVA:
  case OBSTACLE_PRICKLE:
  case OBSTACLE_LADDER:
  case OBSTACLE:
    for (i = 0; i < tile_height8; i++) {
      for (j = 0; j < tile_width8; j++) {
        set_obstacle(layer, tile_x8 + j, tile_y8 + i, obstacle);
      }
    }
    break;

    /* If the top right corner of the tile is an obstacle,
     * then the top right 8*8 squares are OBSTACLE, the bottom left
     * 8*8 squares are OBSTACLE_NONE or OBSTACLE_DEEP_WATER and the 8*8 squares
     * on the diagonal are OBSTACLE_TOP_RIGHT.
     */
  case OBSTACLE_TOP_RIGHT:
  case OBSTACLE_TOP_RIGHT_WATER:

    non_obstacle_triangle = (obstacle == OBSTACLE_TOP_RIGHT) ?
        OBSTACLE_NONE : OBSTACLE_DEEP_WATER;

    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + i, tile_y8 + i, OBSTACLE_TOP_RIGHT);

      // left part of the row: we are in the bottom-left corner
      for (j = 0; j < i; j++) {
        set_obstacle(layer, tile_x8 + j, tile_y8 + i, non_obstacle_triangle);
      }

      // right part of the row: we are in the top-right corner
      for (j = i + 1; j < tile_width8; j++) {
        set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE);
      }
    }
    break;

  case OBSTACLE_TOP_LEFT:
  case OBSTACLE_TOP_LEFT_WATER:

    non_obstacle_triangle = (obstacle == OBSTACLE_TOP_LEFT) ?
        OBSTACLE_NONE : OBSTACLE_DEEP_WATER;

    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // right part of the row: we are in the bottom-right corner
      for (j = tile_width8 - i; j < tile_width8; j++) {
        set_obstacle(layer, tile_x8 + j, tile_y8 + i, non_obstacle_triangle);
      }

      // left part of the row: we are in the top-left corner
      for (j = 0; j < tile_width8 - i - 1; j++) {
        set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE);
      }

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE_TOP_LEFT);
    }
    break;

  case OBSTACLE_BOTTOM_LEFT:
  case OBSTACLE_BOTTOM_LEFT_WATER:

    non_obstacle_triangle = (obstacle == OBSTACLE_BOTTOM_LEFT) ?
        OBSTACLE_NONE : OBSTACLE_DEEP_WATER;

    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // right part of the row: we are in the top-right corner
      for (j = i + 1; j < tile_width8; j++) {
        set_obstacle(layer, tile_x8 + j, tile_y8 + i, non_obstacle_triangle);
      }
      // left part of the row: we are in the bottom-left corner
      for (j = 0; j < i; j++) {
        set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE);
      }

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE_BOTTOM_LEFT);
    }
    break;

  case OBSTACLE_BOTTOM_RIGHT:
  case OBSTACLE_BOTTOM_RIGHT_WATER:

    non_obstacle_triangle = (obstacle == OBSTACLE_BOTTOM_RIGHT) ?
        OBSTACLE_NONE : OBSTACLE_DEEP_WATER;

    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + tile_width8 - i - 1, tile_y8 + i, OBSTACLE_BOTTOM_RIGHT);

      // left part of the row: we are in the top-left corner
      for (j = 0; j < tile_width8 - i - 1; j++) {
        set_obstacle(layer, tile_x8 + j, tile_y8 + i, non_obstacle_triangle);
      }

      // right part of the row: we are in the bottom-right corner
      for (j = tile_width8 - i; j < tile_width8; j++) {
        set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE);
      }
    }
    break;

  case OBSTACLE_EMPTY:
    // keep the obstacle property from any tile already here
    break;
  }
}

/**
 * \brief Adds an entity to the map.
 *
 * This function is called when loading the map. If the entity
 * specified is NULL (because some entity creation functions
 * may return NULL), nothing is done.
 *
 * \param entity the entity to add (can be NULL)
 */
void MapEntities::add_entity(MapEntity* entity) {

  if (entity == NULL) {
    return;
  }

  if (entity->get_type() == TILE) {
    // Tiles are optimized specifically for obstacle checks and rendering.
    add_tile(static_cast<Tile*>(entity));
  }
  else {
    Layer layer = entity->get_layer();

    // update the detectors list
    if (entity->is_detector()) {
      detectors.push_back(static_cast<Detector*>(entity));
    }

    // update the obstacle list
    if (entity->can_be_obstacle()) {

      if (entity->has_layer_independent_collisions()) {
        // some entities handle collisions on any layer (e.g. stairs inside a single floor)
        obstacle_entities[LAYER_LOW].push_back(entity);
        obstacle_entities[LAYER_INTERMEDIATE].push_back(entity);
        obstacle_entities[LAYER_HIGH].push_back(entity);
      }
      else {
        // but usually, an entity collides with only one layer
        obstacle_entities[layer].push_back(entity);
      }
    }

    // update the sprites list
    if (entity->is_drawn_in_y_order()) {
      entities_drawn_y_order[layer].push_back(entity);
    }
    else if (entity->can_be_drawn()) {
      entities_drawn_first[layer].push_back(entity);
    }

    // update the specific entities lists
    switch (entity->get_type()) {

      case STAIRS:
        stairs[layer].push_back(static_cast<Stairs*>(entity));
        break;

      case CRYSTAL_BLOCK:
        crystal_blocks[layer].push_back(static_cast<CrystalBlock*>(entity));
        break;

      case CAMERA_STOPPER:
        camera_stoppers.push_back(static_cast<CameraStopper*>(entity));
        break;

      case BOOMERANG:
        this->boomerang = static_cast<Boomerang*>(entity);
        break;

      default:
      break;
    }

    // update the list of all entities
    all_entities.push_back(entity);
  }

  const std::string& name = entity->get_name();
  if (!name.empty()) {
    Debug::check_assertion(named_entities.find(name) == named_entities.end(),
        StringConcat()
        << "Error: an entity with name '" << name << "' already exists.");
    named_entities[name] = entity;
  }
  entity->increment_refcount();

  // notify the entity
  entity->set_map(map);
}

/**
 * \brief Removes an entity from the map and schedules it to be destroyed.
 * \param entity the entity to remove
 */
void MapEntities::remove_entity(MapEntity* entity) {

  if (!entity->is_being_removed()) {
    entities_to_remove.push_back(entity);
    entity->notify_being_removed();

    if (entity == this->boomerang) {
      this->boomerang = NULL;
    }
  }
}

/**
 * \brief Removes an entity from the map.
 * \param name Name of the entity.
 */
void MapEntities::remove_entity(const std::string& name) {

  MapEntity* entity = find_entity(name);
  if (entity != NULL) {
    remove_entity(entity);
  }
}

/**
 * \brief Removes all entities of a type whose name starts with the specified prefix.
 * \param prefix Prefix of the name of the entities to remove.
 */
void MapEntities::remove_entities_with_prefix(const std::string& prefix) {

  std::list<MapEntity*> entities = get_entities_with_prefix(prefix);
  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    remove_entity(*it);
  }
}

/**
 * \brief Removes and destroys the entities placed in the entities_to_remove list.
 */
void MapEntities::remove_marked_entities() {

  list<MapEntity*>::iterator it;

  // remove the marked entities
  for (it = entities_to_remove.begin();
       it != entities_to_remove.end();
       it++) {

    MapEntity* entity = *it;
    Layer layer = entity->get_layer();

    // remove it from the obstacle entities list if present
    if (entity->can_be_obstacle()) {

      if (entity->has_layer_independent_collisions()) {
        for (int i = 0; i < LAYER_NB; i++) {
          obstacle_entities[i].remove(entity);
        }
      }
      else {
        obstacle_entities[layer].remove(entity);
      }
    }

    // remove it from the detectors list if present
    if (entity->is_detector()) {
      detectors.remove(static_cast<Detector*>(entity));
    }

    // remove it from the sprite entities list if present
    if (entity->is_drawn_in_y_order()) {
      entities_drawn_y_order[layer].remove(entity);
    }
    else if (entity->can_be_drawn()) {
      entities_drawn_first[layer].remove(entity);
    }

    // remove it from the whole list
    all_entities.remove(entity);
    const std::string& name = entity->get_name();
    if (!name.empty()) {
      named_entities.erase(name);
    }

    // update the specific entities lists
    switch (entity->get_type()) {

      case STAIRS:
        stairs[layer].remove(static_cast<Stairs*>(entity));
        break;

      case CRYSTAL_BLOCK:
        crystal_blocks[layer].remove(static_cast<CrystalBlock*>(entity));
        break;

      case CAMERA_STOPPER:
        camera_stoppers.remove(static_cast<CameraStopper*>(entity));
        break;

      case BOOMERANG:
        this->boomerang = NULL;
        break;

      default:
      break;
    }

    // destroy it
    destroy_entity(entity);
  }
  entities_to_remove.clear();
}

/**
 * \brief Suspends or resumes the movement and animations of the entities.
 *
 * This function is called by the map when the game
 * is being suspended or resumed.
 *
 * \param suspended true to suspend the movement and the animations,
 * false to resume them
 */
void MapEntities::set_suspended(bool suspended) {

  // the hero first
  hero.set_suspended(suspended);

  // other entities
  list<MapEntity*>::iterator i;
  for (i = all_entities.begin();
       i != all_entities.end();
       i++) {

    (*i)->set_suspended(suspended);
  }

  // note that we don't suspend the tiles
}

/**
 * \brief Updates the position, movement and animation each entity.
 */
void MapEntities::update() {

  Debug::check_assertion(map.is_started(), "The map is not started");

  // first update the hero
  hero.update();

  // update the tiles and the dynamic entities
  list<MapEntity*>::iterator it;
  for (int layer = 0; layer < LAYER_NB; layer++) {

    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      tiles[layer][i]->update();
    }

    // sort the entities drawn in y order
    entities_drawn_y_order[layer].sort(compare_y);
  }

  for (it = all_entities.begin();
       it != all_entities.end();
       it++) {

    if (!(*it)->is_being_removed()) {
      (*it)->update();
    }
  }

  // remove the entities that have to be removed now
  remove_marked_entities();
}

/**
 * \brief Determines which rectangles are animated and draws all non-animated
 * rectangles of tiles on intermediate surfaces.
 */
void MapEntities::build_non_animated_tiles() {

  const Rectangle map_size(0, 0, map.get_width(), map.get_height());
  for (int layer = 0; layer < LAYER_NB; layer++) {

    delete non_animated_tiles_surfaces[layer];
    non_animated_tiles_surfaces[layer] = new Surface(map_size.get_width(), map_size.get_height());
    non_animated_tiles_surfaces[layer]->set_transparency_color(Color::get_magenta());
    non_animated_tiles_surfaces[layer]->fill_with_color(Color::get_magenta());

    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      Tile& tile = *tiles[layer][i];
      if (!tile.is_animated()) {
        // non-animated tile: optimize its displaying
        tile.draw(*non_animated_tiles_surfaces[layer], map_size);
      }
      else {
        // animated tile: mark its region as non-optimizable
        // (otherwise, a non-animated tile above an animated one would screw us)

        int tile_x8 = tile.get_x() / 8;
        int tile_y8 = tile.get_y() / 8;
        int tile_width8 = tile.get_width() / 8;
        int tile_height8 = tile.get_height() / 8;

        for (int i = 0; i < tile_height8; i++) {
          for (int j = 0; j < tile_width8; j++) {

            int x8 = tile_x8 + j;
            int y8 = tile_y8 + i;
            if (x8 >= 0 && x8 < map_width8 && y8 >= 0 && y8 < map_height8) {
              int index = y8 * map_width8 + x8;
              animated_tiles[layer][index] = true;
            }
          }
        }
      }
    }

    // erase the rectangles that contain animated tiles
    int index = 0;
    for (int y = 0; y < map.get_height(); y += 8) {
      for (int x = 0; x < map.get_width(); x += 8) {

        if (animated_tiles[layer][index]) {
          Rectangle animated_square(x, y, 8, 8);
          non_animated_tiles_surfaces[layer]->fill_with_color(Color::get_magenta(), animated_square);
        }
        index++;
      }
    }

    // build the list of animated tiles and tiles overlapping them
    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      Tile& tile = *tiles[layer][i];
      if (tile.is_animated() || overlaps_animated_tile(tile)) {
        tiles_in_animated_regions[layer].push_back(&tile);
      }
    }
  }
}

/**
 * \brief Draws all non-animated rectangles of tiles on intermediate surfaces.
 *
 * This function is similar to build_non_animated_tiles() except that it
 * assumes that animated and non-animated rectangles were already determined.
 *
 * This function is called when the tileset changes.
 */
void MapEntities::redraw_non_animated_tiles() {

  const Rectangle map_size(0, 0, map.get_width(), map.get_height());
  for (int layer = 0; layer < LAYER_NB; layer++) {

    non_animated_tiles_surfaces[layer]->fill_with_color(Color::get_magenta());

    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      Tile& tile = *tiles[layer][i];
      if (!tile.is_animated()) {
        // Non-animated tile: optimize its displaying.
        tile.draw(*non_animated_tiles_surfaces[layer], map_size);
      }
    }

    // Erase rectangles that contain animated tiles.
    int index = 0;
    for (int y = 0; y < map.get_height(); y += 8) {
      for (int x = 0; x < map.get_width(); x += 8) {

        if (animated_tiles[layer][index]) {
          Rectangle animated_square(x, y, 8, 8);
          non_animated_tiles_surfaces[layer]->fill_with_color(Color::get_magenta(), animated_square);
        }
        index++;
      }
    }
  }
}

/**
 * \brief Returns whether a tile is overlapping an animated other tile.
 * \param tile the tile to check
 * \return true if this tile is overlapping an animated tile
 */
bool MapEntities::overlaps_animated_tile(Tile& tile) {

  bool* animated_tiles_layer = animated_tiles[tile.get_layer()];

  int tile_x8 = tile.get_x() / 8;
  int tile_y8 = tile.get_y() / 8;
  int tile_width8 = tile.get_width() / 8;
  int tile_height8 = tile.get_height() / 8;

  for (int i = 0; i < tile_height8; i++) {
    for (int j = 0; j < tile_width8; j++) {

      int x8 = tile_x8 + j;
      int y8 = tile_y8 + i;
      if (x8 >= 0 && x8 < map_width8 && y8 >= 0 && y8 < map_height8) {

        int index = y8 * map_width8 + x8;
        if (animated_tiles_layer[index]) {
          return true;
        }
      }
    }
  }
  return false;
}

/**
 * \brief Draws the entities on the map surface.
 */
void MapEntities::draw() {

  for (int layer = 0; layer < LAYER_NB; layer++) {

    // draw the animated tiles and the tiles that overlap them:
    // in other words, draw all regions containing animated tiles
    // (and maybe more, but we don't care because non-animated tiles
    // will be drawn later)
    for (unsigned int i = 0; i < tiles_in_animated_regions[layer].size(); i++) {
      tiles_in_animated_regions[layer][i]->draw_on_map();
    }

    // draw the non-animated tiles (with transparent rectangles on the regions of animated tiles
    // since they are already drawn)
    non_animated_tiles_surfaces[layer]->draw_region(
        map.get_camera_position(), map.get_visible_surface());

    // draw the first sprites
    list<MapEntity*>::iterator i;
    for (i = entities_drawn_first[layer].begin();
        i != entities_drawn_first[layer].end();
        i++) {

      MapEntity* entity = *i;
      if (entity->is_enabled()) {
        entity->draw_on_map();
      }
    }

    // draw the sprites at the hero's level, in the order
    // defined by their y position (including the hero)
    for (i = entities_drawn_y_order[layer].begin();
        i != entities_drawn_y_order[layer].end();
        i++) {

      MapEntity* entity = *i;
      if (entity->is_enabled()) {
        entity->draw_on_map();
      }
    }
  }
}

/**
 * \brief Compares the y position of two entities.
 * \param first an entity
 * \param second another entity
 * \return true if the y position of the first entity is lower
 * than the second one
 */
bool MapEntities::compare_y(MapEntity* first, MapEntity* second) {

  // before was: first->get_top_left_y() < second->get_top_left_y(); but doesn't work for bosses
  return first->get_top_left_y() + first->get_height() < second->get_top_left_y() + second->get_height();
}

/**
 * \brief Changes the layer of an entity.
 *
 * Only some specific entities should change their layer.
 *
 * \param entity an entity
 * \param layer the new layer
 */
void MapEntities::set_entity_layer(MapEntity& entity, Layer layer) {

  Layer old_layer = entity.get_layer();

  if (layer != old_layer) {

    // update the obstacle list
    if (entity.can_be_obstacle() && !entity.has_layer_independent_collisions()) {
      obstacle_entities[old_layer].remove(&entity);
      obstacle_entities[layer].push_back(&entity);
    }

    // update the sprites list
    if (entity.is_drawn_in_y_order()) {
      entities_drawn_y_order[old_layer].remove(&entity);
      entities_drawn_y_order[layer].push_back(&entity);
    }
    else if (entity.can_be_drawn()) {
      entities_drawn_first[old_layer].remove(&entity);
      entities_drawn_first[layer].push_back(&entity);
    }

    // update the entity after the lists because this function might be called again
    entity.set_layer(layer);
  }
}

/**
 * \brief Returns whether a rectangle overlaps with a raised crystal block.
 * \param layer the layer to check
 * \param rectangle a rectangle
 * \return true if this rectangle overlaps a raised crystal block
 */
bool MapEntities::overlaps_raised_blocks(Layer layer, const Rectangle& rectangle) {

  bool overlaps = false;
  std::list<CrystalBlock*> blocks = get_crystal_blocks(layer);

  std::list<CrystalBlock*>::iterator it;
  for (it = blocks.begin(); it != blocks.end() && !overlaps; it++) {
    overlaps = (*it)->overlaps(rectangle) && (*it)->is_raised();
  }

  return overlaps;
}

/**
 * \brief Returns true if the player has thrown the boomerang.
 * \return true if the boomerang is present on the map
 */
bool MapEntities::is_boomerang_present() {
  return boomerang != NULL;
}

/**
 * \brief Removes the boomerang from the map, if it is present.
 */
void MapEntities::remove_boomerang() {

  if (boomerang != NULL) {
    remove_entity(boomerang);
    boomerang = NULL;
  }
}

/**
 * \brief Removes any boomerang from the map.
 */
void MapEntities::remove_arrows() {

  // TODO this function may be slow if there are a lot of entities: store the arrows?
  std::list<MapEntity*>::iterator it;
  for (it = all_entities.begin(); it != all_entities.end(); it++) {
    MapEntity* entity = *it;
    if (entity->get_type() == ARROW) {
      remove_entity(entity);
    }
  }
}

