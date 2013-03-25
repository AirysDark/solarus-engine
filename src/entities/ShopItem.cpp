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
#include "entities/ShopItem.h"
#include "entities/Hero.h"
#include "lua/LuaContext.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "DialogBox.h"
#include "Equipment.h"
#include "EquipmentItem.h"
#include "Savegame.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <sstream>

/**
 * @brief Creates a new shop item with the specified treasure and price.
 * @param name the name identifying this entity
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param treasure the treasure that the hero can buy (will be deleted automatically)
 * @param price the treasure's price in rupees
 * @param dialog_id id of the dialog describing the item when the player watches it
 */
ShopItem::ShopItem(const std::string& name, Layer layer, int x, int y,
		   const Treasure& treasure, int price, const std::string& dialog_id):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 32, 32),
  treasure(treasure),
  price(price),
  dialog_id(dialog_id),
  price_digits(0, 0, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP),
  rupee_icon_sprite("entities/rupee_icon"),
  is_looking_item(false),
  is_asking_question(false) {

  std::ostringstream oss;
  oss << price;
  price_digits.set_text(oss.str());
}

/**
 * @brief Destructor.
 */
ShopItem::~ShopItem() {
}

/**
 * @brief Creates a new shop item with the specified treasure and price.
 * @param game the current game
 * @param name the name identifying this entity
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param treasure the treasure that the hero can buy
 * @param price the treasure's price in rupees
 * @param dialog_id id of the dialog describing the item when the player watches it
 * @return the shop item created, or NULL if it is already bought
 */
ShopItem* ShopItem::create(Game& game, const std::string& name, Layer layer, int x, int y,
    const Treasure &treasure, int price, const std::string& dialog_id) {

  // see if the item is not already bought
  if (treasure.is_found()) {
    return NULL;
  }

  return new ShopItem(name, layer, x, y, treasure, price, dialog_id);
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType ShopItem::get_type() {
  return SHOP_ITEM;
}

/**
 * @brief Returns true if this entity does not react to the sword.
 *
 * If true is returned, nothing will happen when the hero taps this entity with the sword.
 *
 * @return true if the sword is ignored
 */
bool ShopItem::is_sword_ignored() {
  return true;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool ShopItem::is_obstacle_for(MapEntity &other) {
  return true;
}

/**
 * @brief This function is called by the engine when an entity overlaps the shop item.
 *
 * If the entity is the hero, we allow him to buy the item.
 *
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void ShopItem::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping.is_hero() && !get_game().is_suspended()) {

    Hero &hero = (Hero&) entity_overlapping;

    if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
        && hero.is_free()) {

      // we show the 'look' icon
      get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * @brief Notifies this detector that the player is interacting with it by
 * pressing the action command.
 *
 * This function is called when the player presses the action command
 * while the hero is facing this detector, and the action command effect lets
 * him do this.
 * A dialog is shown to let the hero buy the item.
 */
void ShopItem::notify_action_command_pressed() {

  if (get_hero().is_free()
      && get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_LOOK) {

    get_dialog_box().start_dialog(dialog_id);
    is_looking_item = true;
  }
}

/**
 * @brief Updates the entity.
 */
void ShopItem::update() {

  if (is_looking_item && !get_game().is_dialog_enabled()) {

    // the description message has just finished
    const std::string question_dialog_id = "_shop.question";
    get_dialog_box().start_dialog(question_dialog_id);
    get_dialog_box().set_variable(question_dialog_id, price);
    is_asking_question = true;
    is_looking_item = false;
  }
  else if (is_asking_question && !get_game().is_dialog_enabled()) {

    // the question has just finished
    is_asking_question = false;
    int answer = get_dialog_box().get_last_answer();

    if (answer == 0) {

      // the player wants to buy the item
      Equipment& equipment = get_equipment();
      EquipmentItem& item = treasure.get_item();

      if (equipment.get_money() < price) {
        // not enough rupees
        Sound::play("wrong");
        get_dialog_box().start_dialog("_shop.not_enough_money");
      }
      else if (item.has_amount() && item.get_amount() >= item.get_max_amount()) {
        // the player already has the maximum amount of this item
        Sound::play("wrong");
        get_dialog_box().start_dialog("_shop.amount_full");
      }
      else {

        bool can_buy = get_lua_context().shop_item_on_buying(*this);
        if (can_buy) {

          // give the treasure
          equipment.remove_money(price);

          get_hero().start_treasure(treasure, LUA_REFNIL);
          if (treasure.is_saved()) {
            remove_from_map();
            get_savegame().set_boolean(treasure.get_savegame_variable(), true);
          }
          get_lua_context().shop_item_on_bought(*this);
        }
      }
    }
  }
}

/**
 * @brief Draws the entity on the map.
 */
void ShopItem::draw_on_map() {

  Surface& map_surface = get_map().get_visible_surface();
  int x = get_x();
  int y = get_y();

  // draw the treasure
  const Rectangle& camera_position = get_map().get_camera_position();
  treasure.draw(map_surface,
      x + 16 - camera_position.get_x(),
      y + 13 - camera_position.get_y());

  // also draw the price
  price_digits.draw(map_surface,
      x + 12 - camera_position.get_x(),
      y + 21 - camera_position.get_y());
  rupee_icon_sprite.draw(map_surface,
      x - camera_position.get_x(),
      y + 22 - camera_position.get_y());
}

