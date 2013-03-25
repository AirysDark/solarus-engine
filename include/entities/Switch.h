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
#ifndef SOLARUS_SWITCH_H
#define SOLARUS_SWITCH_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * @brief A button that the hero can trigger.
 *
 * A switch can be triggered by walking onto it
 * or by using weapons on it, depending on its subtype
 * Some walkable switches require a block to be activated.
 * Some walkable switches become disabled when the hero or the block leave it.
 * A walkable switch can be visible or invisible.
 * An arrow target is always invisible.
 */
class Switch: public Detector {

  public:

    /**
     * Subtypes of switches.
     */
    enum Subtype {
      WALKABLE_INVISIBLE = 0, /**< an invisible switch, typically used to detect the hero position */
      WALKABLE_VISIBLE   = 1, /**< a classical visible switch the hero can walk on */
      ARROW_TARGET       = 2, /**< an invisible switch that can be triggered by shooting an arrow on it */
      SOLID              = 3  /**< a crystal-like switch that can be triggered with the sword */
    };

  private:

    Subtype subtype;                           /**< subtype of switch */
    bool activated;                            /**< indicates that this switch is currently on */
    bool locked;                               /**< indicates that this switch cannot be enabled or disabled by other entities for now */

    // the following fields are used only for walkable switches
    bool needs_block;                          /**< indicates that a block or a statue is required to activate this walkable switch */
    bool inactivate_when_leaving;              /**< indicates that this walkable switch becomes inactivated when the hero or the block leaves it */
    MapEntity* entity_overlapping;             /**< the entity currently on this walkable switch (as arrows may be destroyed at any moment) */
    bool entity_overlapping_still_present;     /**< to detect when the entity overlapping leaves the switch */

  public:

    Switch(const std::string& name, Layer layer, int x, int y,
	Subtype subtype, bool needs_block, bool inactivate_when_leaving);
    ~Switch();

    EntityType get_type();
    bool is_obstacle_for(MapEntity& other);

    bool is_walkable();
    bool is_arrow_target();
    bool is_solid();
    bool is_activated();
    void activate();
    void set_activated(bool enabled);
    void set_locked(bool locked);

    void try_activate(Hero& hero);
    void try_activate(Block& block);
    void try_activate(Arrow& arrow);
    void try_activate();

    void update();
    bool test_collision_custom(MapEntity& entity);
    void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode);
    void notify_collision(MapEntity& other_entity, Sprite& other_sprite, Sprite& this_sprite);

    virtual const std::string& get_lua_type_name() const;
};

#endif

