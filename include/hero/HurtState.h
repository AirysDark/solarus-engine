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
#ifndef SOLARUS_HERO_HURT_STATE_H
#define SOLARUS_HERO_HURT_STATE_H

#include "hero/State.h"

/**
 * @brief The state "Hurt" of the hero.
 */
class Hero::HurtState: public Hero::State {

  private:

    const Rectangle source_xy; /**< coordinates of whatever is hurting the hero (usually an enemy) */
    int life_points;           /**< number of life points to remove (this number may be reduced by the tunic) */
    int magic_points;          /**< number of magic points to remove */
    uint32_t end_hurt_date;    /**< date when the state ends */

  public:

    HurtState(Hero &hero, const Rectangle& source_xy, int life_points, int magic_points);
    ~HurtState();

    void start(State *previous_state);
    void stop(State *next_state);
    void update();
    void set_suspended(bool suspended);
    bool can_start_gameover_sequence();
    bool is_touching_ground();
    bool is_teletransporter_obstacle(Teletransporter &teletransporter);
    bool is_conveyor_belt_obstacle(ConveyorBelt &conveyor_belt);
    bool is_sensor_obstacle(Sensor &sensor);
    bool can_be_hurt(Enemy* attacker);
    bool can_avoid_switch();
};

#endif

