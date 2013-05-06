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
#ifndef SOLARUS_HERO_PUSHING_STATE_H
#define SOLARUS_HERO_PUSHING_STATE_H

#include "hero/State.h"

/**
 * @brief The state "Pushing" of the hero.
 */
class Hero::PushingState: public Hero::State {

  private:

    int pushing_direction4;       /**< direction where the hero is looking (0 to 3) */
    Detector *pushed_entity;      /**< the entity the hero is pushing (or NULL) */

    void stop_moving_pushed_entity();

  public:

    PushingState(Hero &hero);
    ~PushingState();

    void start(State *previous_state);
    void stop(State *next_state);
    void update();
    bool can_avoid_conveyor_belt();
    bool can_start_sword();
    bool is_moving_grabbed_entity();
    void notify_grabbed_entity_collision();
    void notify_movement_finished();
    void notify_obstacle_reached();
    void notify_position_changed();

    bool can_be_hurt(Enemy* attacker);
    bool is_shallow_water_obstacle();
    bool is_deep_water_obstacle();
    bool is_hole_obstacle();
    bool is_lava_obstacle();
    bool is_prickle_obstacle();
};

#endif

