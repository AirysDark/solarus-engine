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
#ifndef SOLARUS_ENEMY_REACTION_H
#define SOLARUS_ENEMY_REACTION_H

#include "Common.h"
#include <string>
#include <map>

/**
 * @brief Describes how an enemy reacts when it receives an attack.
 *
 * The reaction may be different between different sprites of the enemy.
 */
class EnemyReaction {

  public:

    /**
     * @brief Types of reactions to an attack.
     */
    enum ReactionType {
      HURT,              /**< the enemy is hurt and loses some life points */
      IGNORED,           /**< nothing happens */
      PROTECTED,         /**< the attack is stopped */
      IMMOBILIZED,       /**< the enemy is temporarily immobilized */
      CUSTOM,            /**< the enemy's script decides what to do */
      REACTION_NUMBER
    };

    /**
     * @brief Represents a reaction to an attack.
     */
    struct Reaction {
      ReactionType type; /**< type of reaction */
      int life_lost;     /**< number of life points lost (possibly zero) */
    };

  private:

    static const std::string reaction_names[]; /**< name of each reaction type */

    Reaction general_reaction;                 /**< reaction to make unless sprite-specific override */
    std::map<const Sprite*, Reaction> sprite_reactions;  /**< sprite-specific reaction (overrides the default one) */

  public:

    EnemyReaction();
    ~EnemyReaction();

    void set_default_reaction();
    void set_general_reaction(ReactionType reaction, int life_lost = 0);
    void set_sprite_reaction(const Sprite* sprite, ReactionType reaction, int life_lost = 0);
    const Reaction& get_reaction(const Sprite* sprite);

    static const std::string& get_reaction_name(ReactionType reaction);
    static ReactionType get_reaction_by_name(const std::string& name);
};

#endif

