/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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

namespace solarus {

/**
 * \brief The state "Hurt" of the hero.
 */
class Hero::HurtState: public Hero::State {

  public:

    HurtState(Hero& hero,
        const Rectangle* source_xy,
        int damage);
    ~HurtState();

    void start(const State* previous_state);
    void stop(const State* next_state);
    void update();
    void set_suspended(bool suspended);
    bool can_start_gameover_sequence() const;
    bool is_touching_ground() const;
    bool is_teletransporter_obstacle(const Teletransporter& teletransporter) const;
    bool is_stream_obstacle(const Stream& stream) const;
    bool is_sensor_obstacle(const Sensor& sensor) const;
    bool is_separator_obstacle(const Separator& separator) const;
    bool can_be_hurt(MapEntity* attacker) const;
    bool can_avoid_switch() const;
    bool can_avoid_ice() const;

  private:

    bool has_source;              /**< Whether there is a source_xy rectangle.
                                   * If no, the hero does not move. */
    const Rectangle source_xy;    /**< Coordinates of whatever is hurting the
                                   * hero (usually an enemy). */
    int damage;                   /**< Number of life points to remove
                                   * (this number may be reduced by the tunic
                                   * or by hero:on_taking_damage()). */
    uint32_t end_hurt_date;       /**< Date when the state ends. */

};

}

#endif

