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
#include "hero/HeroSprites.h"
#include "entities/Hero.h"
#include "entities/CarriedItem.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "Equipment.h"
#include "Map.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include <sstream>

namespace solarus {

/**
 * \brief Associates to each movement direction the possible directions of the hero's sprites.
 *
 * This array indicates the possible directions of the hero's animation (from 0 to 3, or -1 for no change)
 * for each movement direction.
 * Each combination of directional keys can be associated to one or two directions.
 * When one direction is specified (i.e. the second one is set to -1), the hero's sprite always takes
 * this direction.
 * When two directions are specified,
 * the hero sprite takes the first direction, unless it is already under the second one
 * (then it stays in the second one). This permits a natural behavior for diagonal directions.
 */
const int HeroSprites::animation_directions[][2] = {
  { 0, -1},  // right
  { 0,  1},  // right-up: right or up
  { 1, -1},  // up
  { 2,  1},  // left-up: left or up
  { 2, -1},  // left
  { 2,  3},  // left-down: left or down
  { 3, -1},  // down
  { 0,  3}   // right-down: right or down
};

/**
 * \brief Constructor.
 * \param hero the hero
 * \param equipment the equipment
 */
HeroSprites::HeroSprites(Hero& hero, Equipment& equipment):
  hero(hero),
  equipment(equipment),
  tunic_sprite(NULL),
  sword_sprite(NULL),
  sword_stars_sprite(NULL),
  shield_sprite(NULL),
  shadow_sprite(NULL),
  ground_sprite(NULL),
  trail_sprite(NULL),
  animation_direction_saved(0),
  when_suspended(0),
  blinking(false),
  end_blink_date(0),
  walking(false),
  clipping_rectangle(Rectangle()),
  lifted_item(NULL) {

}

/**
 * \brief Destructor.
 */
HeroSprites::~HeroSprites() {

  delete tunic_sprite;
  delete shadow_sprite;
  delete sword_sprite;
  delete sword_stars_sprite;
  delete shield_sprite;
  delete ground_sprite;
  delete trail_sprite;
}

/**
 * \brief Loads (or reloads) the sprites and sounds of the hero and his equipment.
 *
 * The sprites and sounds loaded depend on his tunic, sword and shield as specified in the savegame.
 * This function must be called at the game beginning
 * and as soon as the hero's equipment is changed.
 */
void HeroSprites::rebuild_equipment() {

  std::string tunic_animation;
  std::string sword_animation;
  std::string shield_animation;
  int animation_direction = -1;

  // the hero
  if (tunic_sprite != NULL) {
    // save the animation direction
    animation_direction = tunic_sprite->get_current_direction();
    tunic_animation = tunic_sprite->get_current_animation();
    delete tunic_sprite;
  }

  int tunic_number = equipment.get_ability(ABILITY_TUNIC);

  Debug::check_assertion(tunic_number > 0, "Invalid tunic number");

  std::ostringstream oss;
  oss << "hero/tunic" << tunic_number;
  tunic_sprite = new Sprite(oss.str());
  tunic_sprite->enable_pixel_collisions();
  if (!tunic_animation.empty()) {
    tunic_sprite->set_current_animation(tunic_animation);
  }

  // the hero's shadow
  if (shadow_sprite == NULL) {
    shadow_sprite = new Sprite("entities/shadow");
    shadow_sprite->set_current_animation("big");
  }

  // the hero's sword
  if (sword_sprite != NULL) {
    if (sword_sprite->is_animation_started()) {
      sword_animation = sword_sprite->get_current_animation();
    }
    delete sword_sprite;
    delete sword_stars_sprite;
    sword_sprite = NULL;
    sword_stars_sprite = NULL;
  }

  int sword_number = equipment.get_ability(ABILITY_SWORD);

  if (sword_number > 0) {
    // the hero has a sword: get the sprite and the sound
    oss.str("");
    oss << "hero/sword" << sword_number;
    sword_sprite = new Sprite(oss.str());
    sword_sprite->enable_pixel_collisions();
    sword_sprite->set_synchronized_to(tunic_sprite);
    if (sword_animation.empty()) {
      sword_sprite->stop_animation();
    }
    else {
      sword_sprite->set_current_animation(sword_animation);
    }

    oss.str("");
    oss << "sword" << sword_number;
    sword_sound_id = oss.str();

    oss.str("");
    oss << "hero/sword_stars" << sword_number;
    sword_stars_sprite = new Sprite(oss.str());
    sword_stars_sprite->stop_animation();
  }

  // the hero's shield
  if (shield_sprite != NULL) {
    if (shield_sprite->is_animation_started()) {
      shield_animation = shield_sprite->get_current_animation();
    }
    delete shield_sprite;
    shield_sprite = NULL;
  }

  int shield_number = equipment.get_ability(ABILITY_SHIELD);

  if (shield_number > 0) {
    // the hero has a shield
    oss.str("");
    oss << "hero/shield" << shield_number;
    shield_sprite = new Sprite(oss.str());
    shield_sprite->set_synchronized_to(tunic_sprite);
    if (shield_animation.empty()) {
      shield_sprite->stop_animation();
    }
    else {
      shield_sprite->set_current_animation(shield_animation);
    }
  }

  // the trail
  trail_sprite = new Sprite("hero/trail");
  trail_sprite->stop_animation();

  // restore the animation direction
  if (animation_direction != -1) {
    set_animation_direction(animation_direction);
  }
}

 /**
 * \brief Returns whether the sword is currently displayed on the screen.
 * \return true if the sword is currently displayed on the screen
 */
bool HeroSprites::is_sword_visible() const {
  return equipment.has_ability(ABILITY_SWORD)
      && sword_sprite != NULL
      && sword_sprite->is_animation_started();
}

/**
 * \brief Returns whether the stars of the sword are currently displayed on the screen.
 * \return true if the stars of the sword are currently displayed on the screen
 */
bool HeroSprites::is_sword_stars_visible() const {
  return equipment.has_ability(ABILITY_SWORD)
      && sword_stars_sprite != NULL
      && sword_stars_sprite->is_animation_started();
}

/**
 * \brief Returns whether the shield is currently displayed on the screen.
 * \return true if the shield is currently displayed on the screen
 */
bool HeroSprites::is_shield_visible() const {
  return equipment.has_ability(ABILITY_SHIELD)
      && shield_sprite != NULL
      && shield_sprite->is_animation_started();
}

/**
 * \brief Returns whether the trail of dust is currently displayed.
 * \return true if the trail of dust is currently displayed
 */
bool HeroSprites::is_trail_visible() const {
  return trail_sprite->is_animation_started();
}

/**
 * \brief Returns whether a special ground is displayed under the hero.
 * \return true if a ground is currently visible under the hero
 */
bool HeroSprites::is_ground_visible() const {
  return hero.is_ground_visible()
      && ground_sprite != NULL;
}

/**
 * \brief Stops displaying the sword and the sword stars (if any).
 */
void HeroSprites::stop_displaying_sword() {

  if (is_sword_visible()) {
    sword_sprite->stop_animation();
  }
  stop_displaying_sword_stars();
}

/**
 * \brief Stops displaying the sword stars (if any).
 */
void HeroSprites::stop_displaying_sword_stars() {

  if (is_sword_stars_visible()) {
    sword_stars_sprite->stop_animation();
  }
}

/**
 * \brief Stops displaying the shield (if any).
 */
void HeroSprites::stop_displaying_shield() {

  if (equipment.has_ability(ABILITY_SHIELD)) {
    shield_sprite->stop_animation();
  }
}

/**
 * \brief Stops displaying the trail (if any).
 */
void HeroSprites::stop_displaying_trail() {
  trail_sprite->stop_animation();
}

/**
 * \brief Makes the hero blink for a while.
 * \param duration Delay before stopping blinking.
 * 0 means infinite.
 */
void HeroSprites::blink(uint32_t duration) {

  const uint32_t blink_delay = 50;

  blinking = true;
  tunic_sprite->set_blinking(blink_delay);

  if (equipment.has_ability(ABILITY_SHIELD)) {
    shield_sprite->set_blinking(blink_delay);
  }
  if (equipment.has_ability(ABILITY_SWORD)) {
    sword_sprite->set_blinking(blink_delay);
  }
  trail_sprite->set_blinking(blink_delay);

  if (duration == 0) {
    // No end date.
    end_blink_date = 0;
  }
  else {
    end_blink_date = System::now() + duration;
  }
}

/**
 * \brief Stops making the hero's sprites blink.
 */
void HeroSprites::stop_blinking() {

  blinking = false;
  end_blink_date = 0;

  tunic_sprite->set_blinking(0);

  if (equipment.has_ability(ABILITY_SHIELD)) {
    shield_sprite->set_blinking(0);
  }
  if (equipment.has_ability(ABILITY_SWORD)) {
    sword_sprite->set_blinking(0);
  }
  trail_sprite->set_blinking(0);
}

/**
 * \brief Returns whether the hero's sprites are currently blinking.
 * \return \c true if the hero's sprites are currently blinking.
 */
bool HeroSprites::is_blinking() const {
  return blinking;
}

/**
 * \brief Sets a rectangle of the map where the drawing of the hero's sprite will be restricted to.
 *
 * A (0,0,0,0) rectangle means that the sprite is not restricted to a subarea of the map.
 *
 * \param clipping_rectangle a subarea of the map to restrict the drawing to
 */
void HeroSprites::set_clipping_rectangle(const Rectangle& clipping_rectangle) {
  this->clipping_rectangle = clipping_rectangle;
}

/**
 * \brief Returns whether the sprites have currently a walking animation.
 * \return true if the sprites are walking
 */
bool HeroSprites::is_walking() const {
  return walking;
}

/**
 * \brief Returns the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * \return the direction of the sprites (0 to 3)
 */
int HeroSprites::get_animation_direction() const {
  return tunic_sprite->get_current_direction();
}

/**
 * \brief Returns the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * \return the direction of the sprites (0 to 7, except diagonal directions)
 */
int HeroSprites::get_animation_direction8() const {
  return get_animation_direction() * 2;
}

/**
 * \brief Returns the direction that the hero's sprites should take.
 *
 * This function returns the direction that the hero's sprites should take depending on the direction wanted
 * by the player and the real movement direction.
 * For diagonal directions, the direction returned depends
 * on the current real direction of the hero's sprites.
 *
 * \param keys_direction the direction defined by the combination of directional keys pressed (0 to 7),
 * or -1 if this is not a valid direction
 * \param real_movement_direction the direction of the hero's actual movement (may be different from keys_direction)
 * \return the direction of the sprites corresponding to these arrows (0 to 3),
 * or -1 if the directional keys combination is illegal
 */
int HeroSprites::get_animation_direction(
    int keys_direction, int real_movement_direction) const {

  int result;

  if (keys_direction == -1) {
    // the player is not pressing a valid combination of directional keys: don't change the sprite's direction
    result = -1;
  }
  else if (keys_direction % 2 == 0) {
    // the player is pressing only one of the four main directions
    // (i.e. he is not trying to make a diagonal move, even if the real movement is diagonal)
    // we just give the sprite this direction
    result = keys_direction / 2;
  }
  // the movement is diagonal: we have to choose between two directions
  else if (animation_directions[real_movement_direction][1] == get_animation_direction()) {
    // we choose the second direction if it was already the sprite's direction
    result = animation_directions[real_movement_direction][1];
  }
  else {
    // otherwise we choose the first direction
    result = animation_directions[real_movement_direction][0];
  }

  return result;
}

/**
 * \brief Changes the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * \param direction the direction to set (0 to 3)
 */
void HeroSprites::set_animation_direction(int direction) {

  Debug::check_assertion(direction >= 0 && direction < 4,
    "Invalid direction for set_animation_direction");

  tunic_sprite->set_current_direction(direction);

  if (is_sword_visible()) {
    sword_sprite->set_current_direction(direction);
  }

  if (is_sword_stars_visible()) {
    sword_stars_sprite->set_current_direction(direction);
  }

  if (is_shield_visible()) {
    shield_sprite->set_current_direction(direction);
  }

  if (is_trail_visible()) {
    trail_sprite->set_current_direction(direction);
  }
}

/**
 * \brief Changes the direction of the hero's sprites.
 *
 * The direction specified is one of the 8 possible movement directions of the hero.
 * The hero's sprites only have four directions, so when
 * the specified direction is a diagonal one,
 * one of the two closest main directions is picked.
 *
 * \param direction the movement direction (0 to 7)
 */
void HeroSprites::set_animation_direction8(int direction) {

  if (get_animation_direction() != animation_directions[direction][1]) {
    set_animation_direction(animation_directions[direction][0]);
  }
}

/**
 * \brief Returns whether the sprites animations are finished.
 * \return true if the animation is finished
 */
bool HeroSprites::is_animation_finished() const {
  return tunic_sprite->is_animation_finished();
}

/**
 * \brief Returns the current frame of the tunic sprite.
 * \return the current frame
 */
int HeroSprites::get_current_frame() const {
  return tunic_sprite->get_current_frame();
}

/**
 * \brief Saves the current direction of the the sprite.
 *
 * Call restore_animation_direction() to restore the direction saved here.
 */
void HeroSprites::save_animation_direction() {
  this->animation_direction_saved = get_animation_direction();
}

/**
 * \brief Restores the direction of the hero's sprite saved by the last
 * call to save_animation_direction().
 */
void HeroSprites::restore_animation_direction() {
  set_animation_direction(animation_direction_saved);
}

/**
 * \brief Updates the animation of the hero's sprites if necessary.
 */
void HeroSprites::update() {

  // update the frames
  tunic_sprite->update();

  if (is_sword_visible()) {
    sword_sprite->update();
    sword_sprite->set_current_frame(tunic_sprite->get_current_frame());
    hero.check_collision_with_detectors(*sword_sprite);
  }
  hero.check_collision_with_detectors(*tunic_sprite);

  if (is_sword_stars_visible()) {
    // the stars are not synchronized with the other sprites
    sword_stars_sprite->update();
  }

  if (is_shield_visible()) {
    shield_sprite->update();
    if (walking) {
      shield_sprite->set_current_frame(tunic_sprite->get_current_frame());
    }
  }

  if (is_trail_visible()) {
    trail_sprite->update();
  }

  if (is_ground_visible()) {
    ground_sprite->update();
  }

  if (lifted_item != NULL && walking) {
    lifted_item->get_sprite().set_current_frame(tunic_sprite->get_current_frame() % 3);
  }

  // blinking
  if (is_blinking()
      && end_blink_date != 0
      && System::now() >= end_blink_date) {
    stop_blinking();
  }
}

/**
 * \brief Draws the hero's sprites on the map.
 */
void HeroSprites::draw_on_map() {

  int x = hero.get_x();
  int y = hero.get_y();

  Map& map = hero.get_map();

  if (hero.is_shadow_visible()) {
    map.draw_sprite(*shadow_sprite, x, y, clipping_rectangle);
  }

  const Rectangle& displayed_xy = hero.get_displayed_xy();
  x = displayed_xy.get_x();
  y = displayed_xy.get_y();

  map.draw_sprite(*tunic_sprite, x, y, clipping_rectangle);

  if (is_trail_visible()) {
    map.draw_sprite(*trail_sprite, x, y, clipping_rectangle);
  }

  if (is_ground_visible()) {
    map.draw_sprite(*ground_sprite, x, y, clipping_rectangle);
  }

  if (is_sword_visible()) {
    map.draw_sprite(*sword_sprite, x, y, clipping_rectangle);
  }

  if (is_sword_stars_visible()) {
    map.draw_sprite(*sword_stars_sprite, x, y, clipping_rectangle);
  }

  if (is_shield_visible()) {
    map.draw_sprite(*shield_sprite, x, y, clipping_rectangle);
  }

  if (lifted_item != NULL) {
    lifted_item->draw_on_map();
  }
}

/**
 * \brief Suspends or resumes the animation of the hero's sprites.
 *
 * This function is called by the map when the game is suspended or resumed.
 *
 * \param suspended true to suspend the hero's sprites, false to resume them
 */
void HeroSprites::set_suspended(bool suspended) {

  tunic_sprite->set_suspended(suspended);

  if (equipment.has_ability(ABILITY_SWORD) && sword_sprite != NULL) {
    sword_sprite->set_suspended(suspended);
    sword_stars_sprite->set_suspended(suspended);
  }

  if (equipment.has_ability(ABILITY_SHIELD) && shield_sprite != NULL) {
    shield_sprite->set_suspended(suspended);
  }

  trail_sprite->set_suspended(suspended);

  if (is_ground_visible()) {
    ground_sprite->set_suspended(suspended);
  }

  // timer
  uint32_t now = System::now();
  if (suspended) {
    when_suspended = now;
  }
  else if (end_blink_date != 0) {
    end_blink_date += now - when_suspended;
  }
}

/**
 * \brief Notifies the hero's sprites that a map has just become active.
 */
void HeroSprites::notify_map_started() {

  // Some sprites may be tileset dependent.
  notify_tileset_changed();
}

/**
 * \brief Notifies the hero's sprites that the tileset has just changed.
 */
void HeroSprites::notify_tileset_changed() {

  // Some sprites may be tileset dependent.
  if (lifted_item != NULL) {
    lifted_item->notify_tileset_changed();
  }

  if (is_ground_visible()) {
    ground_sprite->set_tileset(hero.get_map().get_tileset());
  }
}

/**
 * \brief Restarts the animation of the hero's sprites.
 *
 * This function is called when the sprites have to
 * get back to their first frame.
 */
void HeroSprites::restart_animation() {

  tunic_sprite->restart_animation();

  if (is_sword_visible()) {
    sword_sprite->restart_animation();
  }

  if (is_sword_stars_visible()) {
    sword_stars_sprite->restart_animation();
  }

  if (is_shield_visible()) {
    shield_sprite->restart_animation();
  }

  if (is_trail_visible()) {
    trail_sprite->restart_animation();
  }

  if (is_ground_visible()) {
    ground_sprite->restart_animation();
  }
}

/**
 * \brief Sets whether the hero's sprite should keep playing their animation
 * when the game is suspended.
 * \param ignore_suspend true to make the sprites continue their animation even
 * when the game is suspended
 */
void HeroSprites::set_ignore_suspend(bool ignore_suspend) {

  tunic_sprite->set_ignore_suspend(ignore_suspend);

  if (is_sword_visible()) {
    sword_sprite->set_ignore_suspend(ignore_suspend);
  }

  if (is_sword_stars_visible()) {
    sword_stars_sprite->set_ignore_suspend(ignore_suspend);
  }

  if (is_shield_visible()) {
    shield_sprite->set_ignore_suspend(ignore_suspend);
  }

  if (is_trail_visible()) {
    trail_sprite->set_ignore_suspend(ignore_suspend);
  }

  if (is_ground_visible()) {
    ground_sprite->set_ignore_suspend(ignore_suspend);
  }
}

/**
 * \brief This function is called when the sprites take a "stopped" animation.
 *
 * It makes instructions common to all states having a "stopped" animation.
 * (e.g. free or carrying).
 */
void HeroSprites::set_animation_stopped_common() {

  if (is_ground_visible()
      && hero.get_ground_below() != GROUND_SHALLOW_WATER) {
    ground_sprite->set_current_animation("stopped");
  }
  walking = false;
}

/**
 * \brief Starts the basic "stopped" animation of the hero's sprites.
 */
void HeroSprites::set_animation_stopped_normal() {

  set_animation_stopped_common();

  if (equipment.has_ability(ABILITY_SHIELD)) {

    tunic_sprite->set_current_animation("stopped_with_shield");
    shield_sprite->set_current_animation("stopped");
    shield_sprite->set_current_direction(get_animation_direction());
  }
  else {
    tunic_sprite->set_current_animation("stopped");
  }
  stop_displaying_sword();
  stop_displaying_trail();
}

/**
 * \brief Starts the "stopped" animation of the hero's sprites with the sword loading.
 */
void HeroSprites::set_animation_stopped_sword_loading() {

  set_animation_stopped_common();

  int direction = get_animation_direction();

  tunic_sprite->set_current_animation("sword_loading_stopped");
  sword_sprite->set_current_animation("sword_loading_stopped");
  sword_sprite->set_current_direction(direction);
  sword_stars_sprite->set_current_animation("loading");
  sword_stars_sprite->set_current_direction(direction);

  if (equipment.has_ability(ABILITY_SHIELD)) {

    shield_sprite->set_current_animation("sword_loading_stopped");
    shield_sprite->set_current_direction(direction);
  }
  stop_displaying_trail();
}

/**
 * \brief Starts the "stopped" animation with sprites that represent
 * the hero carrying something.
 *
 * If the hero actually carries an item, the carried item also takes a "stopped" animation.
 */
void HeroSprites::set_animation_stopped_carrying() {

  set_animation_stopped_common();
  tunic_sprite->set_current_animation("carrying_stopped");

  if (lifted_item != NULL) {
    lifted_item->set_animation_stopped();
  }
  stop_displaying_trail();
}

/**
 * \brief Starts the "stopped" animation with sprites that represent
 * the hero swimming.
 */
void HeroSprites::set_animation_stopped_swimming() {

  set_animation_stopped_common();
  tunic_sprite->set_current_animation("swimming_stopped");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief This function is called when the sprites take a "walking" animation.
 *
 * It makes instructions common to all states having a "walking" animation.
 * (e.g. free or carrying).
 */
void HeroSprites::set_animation_walking_common() {

  if (is_ground_visible() && hero.get_ground_below() != GROUND_SHALLOW_WATER) {
    ground_sprite->set_current_animation("walking");
  }

  walking = true;
}

/**
 * \brief Starts the normal "walking" animation of the hero's sprites.
 */
void HeroSprites::set_animation_walking_normal() {

  set_animation_walking_common();

  if (equipment.has_ability(ABILITY_SHIELD)) {

    tunic_sprite->set_current_animation("walking_with_shield");

    shield_sprite->set_current_animation("walking");
    shield_sprite->set_current_direction(get_animation_direction());
  }
  else {
    tunic_sprite->set_current_animation("walking");
  }
  stop_displaying_sword();
  stop_displaying_trail();
}

/**
 * \brief Starts the "walking" animation of the hero's sprites with the sword loading.
 */
void HeroSprites::set_animation_walking_sword_loading() {

  set_animation_walking_common();

  int direction = get_animation_direction();

  tunic_sprite->set_current_animation("sword_loading_walking");
  if (equipment.has_ability(ABILITY_SWORD)) {
    sword_sprite->set_current_animation("sword_loading_walking");
    sword_sprite->set_current_direction(direction);
    sword_stars_sprite->set_current_animation("loading");
    sword_stars_sprite->set_current_direction(direction);
  }

  if (equipment.has_ability(ABILITY_SHIELD)) {
    shield_sprite->set_current_animation("sword_loading_walking");
    shield_sprite->set_current_direction(direction);
  }
  stop_displaying_trail();
}

/**
 * \brief Starts the "walking" animation with sprites that represent
 * the hero carrying something.
 *
 * If the hero actually carries an item, the carried item also takes a "walking" animation.
 */
void HeroSprites::set_animation_walking_carrying() {

  set_animation_walking_common();

  tunic_sprite->set_current_animation("carrying_walking");

  if (lifted_item != NULL) {
    lifted_item->set_animation_walking();
  }
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "swimming_slow" animation of the sprites.
 */
void HeroSprites::set_animation_swimming_slow() {

  set_animation_walking_common();

  tunic_sprite->set_current_animation("swimming_slow");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "swimming_fast" animation of the sprites.
 */
void HeroSprites::set_animation_swimming_fast() {

  set_animation_walking_common();

  tunic_sprite->set_current_animation("swimming_fast");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "walking_diagonal" animation of the hero's sprites.
 * \param direction8 the diagonal direction to take (1, 3, 5 or 7)
 */
void HeroSprites::set_animation_walking_diagonal(int direction8) {

  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
  tunic_sprite->set_current_animation("walking_diagonal");
  tunic_sprite->set_current_direction(direction8 / 2);
}

/**
 * \brief Starts (or restarts) the "sword" animation of the hero's sprites.
 */
void HeroSprites::set_animation_sword() {

  int direction = get_animation_direction();

  tunic_sprite->set_current_animation("sword");
  tunic_sprite->restart_animation();

  sword_sprite->set_current_animation("sword");
  sword_sprite->set_current_direction(direction);
  sword_sprite->restart_animation();
  sword_stars_sprite->stop_animation();

  if (equipment.has_ability(ABILITY_SHIELD)) {

    if (direction % 2 != 0) {
      shield_sprite->set_current_animation("sword");
      shield_sprite->set_current_direction(direction / 2);
      shield_sprite->restart_animation();
    }
    else {
      stop_displaying_shield();
    }
  }
  stop_displaying_trail();
}

/**
 * \brief Plays the sound corresponding to the current sword.
 */
void HeroSprites::play_sword_sound() {
  Sound::play(sword_sound_id);
}

/**
 * \brief Starts (or restarts) the "sword tapping" animation of the hero's sprites.
 */
void HeroSprites::set_animation_sword_tapping() {

  int direction = get_animation_direction();

  tunic_sprite->set_current_animation("sword_tapping");
  tunic_sprite->restart_animation();

  sword_sprite->set_current_animation("sword_tapping");
  sword_sprite->set_current_direction(direction);
  sword_sprite->restart_animation();
  sword_stars_sprite->stop_animation();

  if (equipment.has_ability(ABILITY_SHIELD)) {

    shield_sprite->set_current_animation("sword_tapping");
    shield_sprite->set_current_direction(direction);
    shield_sprite->restart_animation();
  }
  stop_displaying_trail();
}

/**
 * \brief Starts (or restarts) the "spin_attck" animation of the hero's sprites.
 */
void HeroSprites::set_animation_spin_attack() {

  tunic_sprite->set_current_animation("spin_attack");
  sword_sprite->set_current_animation("spin_attack");
  stop_displaying_sword_stars();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts (or restarts) the "super_spin_attck" animation of the hero's sprites.
 */
void HeroSprites::set_animation_super_spin_attack() {

  tunic_sprite->set_current_animation("super_spin_attack");
  sword_sprite->set_current_animation("super_spin_attack");
  stop_displaying_sword_stars();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "grabbing" animation of the hero's sprites.
 */
void HeroSprites::set_animation_grabbing() {

  tunic_sprite->set_current_animation("grabbing");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "pulling" animation of the hero's sprites.
 */
void HeroSprites::set_animation_pulling() {

  tunic_sprite->set_current_animation("pulling");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "pushing" animation of the hero's sprites.
 */
void HeroSprites::set_animation_pushing() {

  tunic_sprite->set_current_animation("pushing");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "lifting" animation of the hero's sprites.
 */
void HeroSprites::set_animation_lifting() {

  tunic_sprite->set_current_animation("lifting");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "jumping" animation of the hero's sprites.
 */
void HeroSprites::set_animation_jumping() {

  tunic_sprite->set_current_animation("jumping");

  if (equipment.has_ability(ABILITY_SHIELD)) {
    shield_sprite->set_current_animation("stopped");
    shield_sprite->set_current_direction(get_animation_direction());
  }
  stop_displaying_sword();
  stop_displaying_trail();
}

/**
 * \brief Starts the "hurt" animation of the hero's sprites.
 */
void HeroSprites::set_animation_hurt() {

  tunic_sprite->set_current_animation("hurt");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "falling" animation of the hero's sprites.
 */
void HeroSprites::set_animation_falling() {

  // show the animation
  tunic_sprite->set_current_animation("falling");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "brandish" animation of the hero's sprites.
 */
void HeroSprites::set_animation_brandish() {

  tunic_sprite->set_current_animation("brandish");
  tunic_sprite->set_current_direction(1);
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "victory" animation of the hero's sprites.
 */
void HeroSprites::set_animation_victory() {

  tunic_sprite->set_current_animation("victory");
  tunic_sprite->set_current_direction(1);
  if (sword_sprite != NULL) {
    sword_sprite->set_current_animation("victory");
    sword_sprite->set_current_direction(1);
  }
  stop_displaying_sword_stars();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "prepare running" animation of the hero's sprites.
 */
void HeroSprites::set_animation_prepare_running() {

  set_animation_walking_normal();
  trail_sprite->set_current_animation("running");
}

/**
 * \brief Starts the "running" animation of the hero's sprites.
 */
void HeroSprites::set_animation_running() {

  set_animation_walking_sword_loading();
  stop_displaying_sword_stars();
  trail_sprite->set_current_animation("running");
}

/**
 * \brief Starts a custom animation of the hero's sprites.
 *
 * The animation of the tunic and the shield (if any) can be specified here.
 * Other sprites are hidden. Many simple animations can be started with
 * this function. More complex one have dedicated functions.
 *
 * \param tunic_animation name of the animation to give to the tunic sprite
 * \param shield_animation name of the animation to give to the shield sprite,
 * or an empty string to hide the shield.
 */
void HeroSprites::set_animation(const std::string& tunic_animation,
    const std::string& shield_animation) {

  tunic_sprite->set_current_animation(tunic_animation);

  if (shield_animation.size() > 0
      && equipment.has_ability(ABILITY_SHIELD)) {
    shield_sprite->set_current_animation(shield_animation);
  }
  else {
    stop_displaying_shield();
  }
  stop_displaying_sword();
  stop_displaying_trail();
}

/**
 * \brief Creates the ground sprite and sound corresponding to the specified
 * ground.
 * \param ground A ground.
 */
void HeroSprites::create_ground(Ground ground) {

  delete ground_sprite;
  ground_sprite = NULL;

  std::string sprite_id;
  if (ground == GROUND_GRASS) {
    sprite_id = "hero/ground1";
    ground_sound_id = "walk_on_grass";
  }
  else if (ground == GROUND_SHALLOW_WATER) {
    sprite_id = "hero/ground2";
    ground_sound_id = "walk_on_water";
  }

  if (!sprite_id.empty()) {
    ground_sprite = new Sprite(sprite_id);
    ground_sprite->set_tileset(hero.get_map().get_tileset());
    if (ground != GROUND_SHALLOW_WATER) {
      ground_sprite->set_current_animation(walking ? "walking" : "stopped");
    }
  }
}

/**
 * \brief Deletes the ground sprite.
 */
void HeroSprites::destroy_ground() {

  delete ground_sprite;
  ground_sprite = NULL;
}

/**
 * \brief Plays a sound for the ground displayed under the hero.
 */
void HeroSprites::play_ground_sound() {
  Sound::play(ground_sound_id);
}

/**
 * \brief Indicates a lifted item to display with the hero's sprites.
 *
 * Calling this function makes this class display the lifted item and
 * give it the appropriate animation, so that you don't have to make it yourself.
 * However, this class only handles displaying:
 * you still have to update it and make it follow the hero.
 *
 * \param lifted_item the item to display, or NULL to stop displaying a lifted item
 */
void HeroSprites::set_lifted_item(CarriedItem *lifted_item) {
  this->lifted_item = lifted_item;
}

}

