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
#ifndef SOLARUS_HERO_SWIMMING_STATE_H
#define SOLARUS_HERO_SWIMMING_STATE_H

#include "hero/PlayerMovementState.h"

/**
 * @brief The state "Swimming" of the hero.
 */
class Hero::SwimmingState: public Hero::PlayerMovementState {

  private:

    bool fast_swimming;            /**< indicates that the hero is currently swimming faster */
    uint32_t end_fast_swim_date;   /**< date when the fast swimming stops */

    int get_slow_swimming_speed();
    int get_fast_swimming_speed();
    void try_swim_faster();

  public:

    SwimmingState(Hero& hero);
    ~SwimmingState();

    void start(State* previous_state);
    void stop(State* next_state);
    void update();
    void set_suspended(bool suspended);

    void set_animation_stopped();
    void set_animation_walking();

    void notify_action_command_pressed();
    void notify_attack_command_pressed();

    bool is_stairs_obstacle(Stairs& stairs);
    bool can_take_jumper();
};

#endif

