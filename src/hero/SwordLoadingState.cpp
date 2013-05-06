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
#include "hero/SwordLoadingState.h"
#include "hero/SpinAttackState.h"
#include "hero/SwordTappingState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "entities/Enemy.h"
#include "lowlevel/System.h"
#include "lowlevel/Sound.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Geometry.h"
#include "Game.h"
#include "GameCommands.h"
#include <sstream>

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::SwordLoadingState::SwordLoadingState(Hero& hero):
  PlayerMovementState(hero) {

}

/**
 * @brief Destructor.
 */
Hero::SwordLoadingState::~SwordLoadingState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::SwordLoadingState::start(State* previous_state) {

  PlayerMovementState::start(previous_state);

  sword_loaded = false;
  sword_loaded_date = System::now() + 1000;
}

/**
 * @brief Updates this state.
 */
void Hero::SwordLoadingState::update() {

  PlayerMovementState::update();

  if (suspended) {
    return;
  }

  uint32_t now = System::now();

  // detect when the sword is loaded (i.e. ready for a spin attack)
  if (!sword_loaded && now >= sword_loaded_date) {
    play_load_sound();
    sword_loaded = true;
  }

  if (!get_commands().is_command_pressed(GameCommands::ATTACK)) {
    // the player has just released the sword key

    // stop loading the sword, go to the normal state or make a spin attack
    if (!sword_loaded) {
      // the sword was not loaded yet: go to the normal state
      hero.set_state(new FreeState(hero));
    }
    else {
      // the sword is loaded: release a spin attack
      hero.set_state(new SpinAttackState(hero));
    }
  }
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::SwordLoadingState::set_suspended(bool suspended) {

  PlayerMovementState::set_suspended(suspended);

  if (!suspended) {
    sword_loaded_date += System::now() - when_suspended;
  }
}

/**
 * @brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 */
void Hero::SwordLoadingState::notify_obstacle_reached() {

  PlayerMovementState::notify_obstacle_reached();

  Detector* facing_entity = hero.get_facing_entity();

  if (hero.is_facing_point_on_obstacle()     // he is really facing an obstacle
      && get_wanted_movement_direction8() == get_sprites().get_animation_direction8()   // he is trying to move towards the obstacle
      && (facing_entity == NULL || !facing_entity->is_sword_ignored())) {               // the obstacle allows him to tap with his sword

    hero.set_state(new SwordTappingState(hero));
  }
}

/**
 * @brief Notifies this state that the hero has just attacked an enemy
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack (see Enemy.h)
 * @param killed indicates that the attack has just killed the enemy
 */
void Hero::SwordLoadingState::notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
    EnemyReaction::Reaction& result, bool killed) {

  if (result.type != EnemyReaction::IGNORED && attack == ATTACK_SWORD) {

    if (victim.get_push_hero_on_sword()) {
      // let SwordTappingState do the job so that no player movement interferes
      State* state = new SwordTappingState(hero);
      hero.set_state(state);
      state->notify_attacked_enemy(attack, victim, result, killed);
    }
    else {
      // after an attack, stop loading the sword
      hero.set_state(new FreeState(hero));
    }
  }
}

/**
 * @brief Returns whether the animation direction is locked.
 * @return true if the animation direction is locked
 */
bool Hero::SwordLoadingState::is_direction_locked() {
  return true;
}

/**
 * @brief Returns whether can trigger a jumper in this state.
 * If false is returned, jumpers have no effect (but they are obstacle for the hero).
 * @return true if the hero can use jumpers in this state
 */
bool Hero::SwordLoadingState::can_take_jumper() {
  return true;
}

/**
 * @brief Returns whether the hero can take stairs in this state.
 * If false is returned, stairs have no effect (but they are obstacle for the hero).
 * @return true if the hero ignores the effect of stairs in this state
 */
bool Hero::SwordLoadingState::can_take_stairs() {
  return true;
}

/**
 * @brief Gives the sprites the animation stopped corresponding to this state.
 */
void Hero::SwordLoadingState::set_animation_stopped() {
  get_sprites().set_animation_stopped_sword_loading();
}

/**
 * @brief Gives the sprites the animation walking corresponding to this state.
 */
void Hero::SwordLoadingState::set_animation_walking() {
  get_sprites().set_animation_walking_sword_loading();
}

/**
 * @brief Plays the sword loading sound.
 */
void Hero::SwordLoadingState::play_load_sound() {

  std::ostringstream oss;
  oss << "sword_spin_attack_load_" << get_equipment().get_ability("sword");
  std::string custom_sound_name = oss.str();
  if (Sound::exists(custom_sound_name)) {
    Sound::play(custom_sound_name); // this particular sword has a custom loading sound effect
  }
  else {
    Sound::play("sword_spin_attack_load");
  }
}

