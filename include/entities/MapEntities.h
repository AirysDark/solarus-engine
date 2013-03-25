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
#ifndef SOLARUS_MAP_ENTITIES_H
#define SOLARUS_MAP_ENTITIES_H

#include "Common.h"
#include "Transition.h"
#include "entities/Obstacle.h"
#include "entities/Layer.h"
#include "entities/EntityType.h"
#include "entities/Enemy.h"
#include <vector>
#include <list>

/**
 * @brief Manages the whole content of a map.
 *
 * Each element of a map is called an entity and is an instance of
 * a subclass of MapEntity.
 * This class stores all entities of the current map:
 * the tiles, the hero, the enemies and all other entities.
 */
class MapEntities {

  public:

    // creation and destruction
    MapEntities(Game& game, Map& map);
    ~MapEntities();

    // entities
    Hero& get_hero();
    Obstacle get_obstacle_tile(Layer layer, int x, int y);
    std::list<MapEntity*>& get_obstacle_entities(Layer layer);
    std::list<Detector*>& get_detectors();
    std::list<Stairs*>& get_stairs(Layer layer);
    std::list<CrystalBlock*>& get_crystal_blocks(Layer layer);

    MapEntity* get_entity(const std::string& name);
    MapEntity* find_entity(const std::string& name);
    std::list<MapEntity*> get_entities_with_prefix(const std::string& prefix);
    std::list<MapEntity*> get_entities_with_prefix(EntityType type, const std::string& prefix);
    bool has_entity_with_prefix(const std::string& prefix);

    // handle entities
    void add_entity(MapEntity* entity);
    void remove_entity(MapEntity* entity);
    void remove_entity(const std::string& name);
    void remove_entities_with_prefix(const std::string& prefix);
    void bring_to_front(MapEntity* entity);
    void destroy_all_entities();
    void destroy_entity(MapEntity* entity);
    static bool compare_y(MapEntity* first, MapEntity* second);
    void set_entity_layer(MapEntity& entity, Layer layer);

    // specific to some entity types
    bool overlaps_raised_blocks(Layer layer, const Rectangle& rectangle);
    bool is_boomerang_present();
    void remove_boomerang();
    void remove_arrows();

    // map events
    void notify_map_started();
    void notify_map_opening_transition_finished();
    void notify_tileset_changed();

    // game loop
    void set_suspended(bool suspended);
    void update();
    void draw();

  private:

    friend class MapLoader;            /**< the map loader initializes the private fields of MapEntities */

    void add_tile(Tile *tile);
    void set_obstacle(int layer, int x8, int y8, Obstacle obstacle);
    void build_non_animated_tiles();
    void redraw_non_animated_tiles();
    bool overlaps_animated_tile(Tile& tile);
    void remove_marked_entities();
    void update_crystal_blocks();

    // map
    Game& game;                                     /**< the game running this map */
    Map& map;                                       /**< the map */
    int map_width8;                                 /**< number of 8*8 squares on a row of the map grid */
    int map_height8;                                /**< number of 8*8 squares on a column of the map grid */

    // tiles
    std::vector<Tile*> tiles[LAYER_NB];             /**< all tiles of the map (a vector for each layer) */
    int tiles_grid_size;                            /**< number of 8*8 squares in the map
                                                     * (tiles_grid_size = map_width8 * map_height8) */
    Obstacle* obstacle_tiles[LAYER_NB];				/**< array of size tiles_grid_size representing which squares
                                                     * are obstacles and how */
    bool* animated_tiles[LAYER_NB];                 /**< array of size tiles_grid_size that remembers which squares
                                                     * have animated tiles */
    Surface* non_animated_tiles_surfaces[LAYER_NB]; /**< all non-animated tiles are rendered once for all on these surfaces
                                                     * for performance */
    std::vector<Tile*>
        tiles_in_animated_regions[LAYER_NB];        /**< animated tiles and tiles overlapping them */

    // dynamic entities
    Hero& hero;                                     /**< the hero (also stored in Game because it is kept when changing maps) */

    std::map<std::string, MapEntity*>
      named_entities;                               /**< entities identified by a name */
    std::list<MapEntity*> all_entities;             /**< all map entities except the tiles and the hero;
                                                     * this vector is used to delete the entities
                                                     * when the map is unloaded */
    std::list<MapEntity*> entities_to_remove;       /**< list of entities that need to be removed right now */

    std::list<MapEntity*>
      entities_drawn_first[LAYER_NB];               /**< all map entities that are drawn in the normal order */

    std::list<MapEntity*>
      entities_drawn_y_order[LAYER_NB];             /**< all map entities that are drawn in the order
                                                     * defined by their y position, including the hero */

    std::list<Detector*> detectors;                 /**< all entities able to detect other entities
                                                     * on this map */

    std::list<MapEntity*>
      obstacle_entities[LAYER_NB];                  /**< all entities that might be obstacle for other
                                                     * entities on this map, including the hero */

    std::list<Stairs*> stairs[LAYER_NB];            /**< all stairs of the map */
    std::list<CrystalBlock*>
      crystal_blocks[LAYER_NB];	                    /**< all crystal blocks of the map */

    Boomerang* boomerang;                           /**< the boomerang if present on the map, NULL otherwise */
    std::string music_before_miniboss;              /**< the music that was played before starting a miniboss fight */
};

/**
 * @brief Returns the obstacle property of the tile located at a specified point.
 *
 * This function assumes that the parameters are correct: for performance reasons,
 * no check is done here.
 * Dynamic tiles are not considered here.
 *
 * @param layer of the tile to get
 * @param x x coordinate of the point
 * @param y y coordinate of the point
 * @return the obstacle property of this tile
 */
inline Obstacle MapEntities::get_obstacle_tile(Layer layer, int x, int y) {

  // warning: this function is called very often so it has been optimized and should remain so

  // optimization of: return obstacle_tiles[layer][(y / 8) * map_width8 + (x / 8)];
  return obstacle_tiles[layer][(y >> 3) * map_width8 + (x >> 3)];
}

#endif

