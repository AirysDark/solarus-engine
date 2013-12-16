/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "DialogBox.h"
#include "DialogResource.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"
#include "entities/Hero.h"
#include "lowlevel/TextSurface.h"
#include "lua/LuaContext.h"

namespace solarus {

/**
 * \brief Creates a new dialog box.
 * \param game The game this dialog box belongs to.
 */
DialogBox::DialogBox(Game& game):
  game(game),
  callback_ref(LUA_REFNIL),
  built_in(false),
  is_question(false),
  selected_first_answer(true) {

  for (int i = 0; i < nb_visible_lines; i++) {
    line_surfaces[i] = new TextSurface(0, 0,
        TextSurface::ALIGN_LEFT, TextSurface::ALIGN_BOTTOM);
  }
}

/**
 * \brief Destructor.
 */
DialogBox::~DialogBox() {

  for (int i = 0; i < nb_visible_lines; i++) {
    delete line_surfaces[i];
  }
  game.get_lua_context().cancel_callback(callback_ref);
}

/**
 * \brief Returns the game where this dialog box is displayed.
 * \return the current game
 */
Game& DialogBox::get_game() {
  return game;
}

/**
 * \brief Returns whether the dialog box is currently active.
 * \return true if the dialog box is enabled
 */
bool DialogBox::is_enabled() const {

  return !dialog_id.empty();
}

/**
 * \brief Returns the id of the current dialog.
 * \return the id of the dialog currently shown
 */
const std::string& DialogBox::get_dialog_id() const {
  return dialog_id;
}

/**
 * \brief Opens the dialog box to show a dialog.
 *
 * No other dialog should be already running.
 *
 * \param dialog_id Id of the dialog to show.
 * \param info_ref Lua ref to an optional info parameter to pass to the
 * dialog box, or LUA_REFNIL.
 * \param callback_ref Lua ref to a function to call when the dialog finishes,
 * or LUA_REFNIL.
 */
void DialogBox::open(const std::string& dialog_id,
    int info_ref, int callback_ref) {

  Debug::check_assertion(!is_enabled(), "A dialog is already active");

  this->dialog_id = dialog_id;
  this->dialog = DialogResource::get_dialog(dialog_id);
  this->callback_ref = callback_ref;

  // Save commands.
  KeysEffect& keys_effect = game.get_keys_effect();
  keys_effect.save_action_key_effect();
  keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  keys_effect.save_sword_key_effect();
  keys_effect.set_sword_key_effect(KeysEffect::SWORD_KEY_NONE);
  keys_effect.save_pause_key_effect();
  keys_effect.set_pause_key_effect(KeysEffect::PAUSE_KEY_NONE);

  // A dialog was just started: notify Lua.
  LuaContext& lua_context = game.get_lua_context();
  lua_State* l = lua_context.get_internal_state();
  built_in = !lua_context.notify_dialog_started(
      game, dialog, info_ref);

  if (built_in) {

    // Show a built-in default dialog box.
    keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_NEXT);

    // Prepare the text.
    std::string text = dialog.get_text();
    this->is_question = false;

    if (dialog_id == "_shop.question") {
      // Built-in dialog with the "do you want to buy" question and the price.
      this->is_question = true;
      size_t index = text.find("$v");
      if (index != std::string::npos) {
        // Replace the special sequence '$v' by the price of the shop item.
        lua_rawgeti(l, LUA_REGISTRYINDEX, info_ref);
        int price = luaL_checkint(l, -1);
        lua_pop(l, -1);
        std::ostringstream oss;
        oss << price;
        text = text.replace(index, 2, oss.str());
      }
    }
    luaL_unref(l, LUA_REGISTRYINDEX, info_ref);

    remaining_lines.clear();
    std::istringstream iss(text);
    std::string line;
    while (std::getline(iss, line)) {
      remaining_lines.push_back(line);
    }

    // Determine the position.
    const Rectangle& camera_position = game.get_current_map().get_camera_position();
    bool top = false;
    if (game.get_hero().get_y() >= camera_position.get_y() + 130) {
      top = true;
    }
    int x = camera_position.get_width() / 2 - 110;
    int y = top ? 32 : camera_position.get_height() - 96;

    text_position.set_xy(x, y);

    // Start showing text.
    show_more_lines();
  }
}

/**
 * \brief Closes the dialog box.
 * \param status_ref Lua ref to a status value to return to the start_dialog
 * callback, or LUA_REFNIL. "skipped" means that the dialog was canceled by
 * the user.
 */
void DialogBox::close(int status_ref) {

  Debug::check_assertion(is_enabled(), "No dialog is active");

  int callback_ref = this->callback_ref;
  this->callback_ref = LUA_REFNIL;
  this->dialog_id = "";

  // Restore commands.
  KeysEffect& keys_effect = game.get_keys_effect();
  keys_effect.restore_action_key_effect();
  keys_effect.restore_sword_key_effect();
  keys_effect.restore_pause_key_effect();

  // A dialog was just finished: notify Lua.
  game.get_lua_context().notify_dialog_finished(
      game, dialog, callback_ref, status_ref);
}

/**
 * \brief Returns whether there are more lines remaining to display after the
 * current 3 lines in the built-in dialog box.
 * \return \c true if there are more lines.
 */
bool DialogBox::has_more_lines() const {
  return !remaining_lines.empty();
}

/**
 * \brief Shows a new group of 3 lines (if possible) in the built-in
 * dialog box.
 */
void DialogBox::show_more_lines() {

  // This function is only called in the built-in case.
  Debug::check_assertion(built_in, "This dialog box is not the built-in one");

  if (!has_more_lines()) {

    int status_ref = LUA_REFNIL;
    if (is_question) {
      // Send the answer to the callback.
      LuaContext& lua_context = game.get_lua_context();
      lua_pushboolean(lua_context.get_internal_state(), selected_first_answer);
      status_ref = lua_context.create_ref();
    }
    close(status_ref);
    return;
  }

  KeysEffect& keys_effect = game.get_keys_effect();
  keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_NEXT);

  // Prepare the 3 lines.
  int text_x = text_position.get_x();
  int text_y = text_position.get_y();
  for (int i = 0; i < nb_visible_lines; i++) {
    text_y += 16;
    line_surfaces[i]->set_x(text_x);
    line_surfaces[i]->set_y(text_y);
    line_surfaces[i]->set_text_color(Color::get_white());

    if (has_more_lines()) {
      line_surfaces[i]->set_text(*remaining_lines.begin());
      remaining_lines.pop_front();
    }
    else {
      line_surfaces[i]->set_text("");
    }
  }

  if (built_in && is_question && !has_more_lines()) {
    // If the dialog is a question, we assume that the last group of 3 lines
    // are the question and the two possible answers.
    // Remember that this is only the case of the built-in dialog box:
    // if the user needs something more elaborate, he should make his own
    // dialog box in Lua.
    this->selected_first_answer = true;
    line_surfaces[nb_visible_lines - 2]->set_text_color(Color::get_yellow());
  }
}

/**
 * \brief This function is called by the game when a command is pressed
 * while a dialog is active.
 *
 * Nothing happens if the dialog is handled in Lua.
 *
 * \param command The command pressed.
 * \return \c true if the command was handled (that is, if the dialog box
 * is active and is the built-in one).
 */
bool DialogBox::notify_command_pressed(GameCommands::Command command) {

  if (!is_enabled()) {
    return false;
  }

  if (!built_in) {
    // The dialog box is handled by a Lua script.
    return false;
  }

  if (command == GameCommands::ACTION) {
    show_more_lines();
  }
  else if (command == GameCommands::UP || command == GameCommands::DOWN) {
    if (is_question && !has_more_lines()) {
      // Switch the selected answer.
      selected_first_answer = !selected_first_answer;
      int selected_line_index = selected_first_answer ? 1 : 2;
      for (int i = 0; i < nb_visible_lines; i++) {
        line_surfaces[i]->set_text_color(Color::get_white());
      }
      line_surfaces[selected_line_index]->set_text_color(Color::get_yellow());
    }
  }

  return true;
}

/**
 * \brief Draws the dialog box on a surface.
 *
 * Draws nothing if the dialog is handled by Lua.
 *
 * \param dst_surface The destination surface.
 */
void DialogBox::draw(Surface& dst_surface) {

  if (!built_in) {
    // The dialog box is handled by a Lua script.
    return;
  }

  // Draw the text.
  for (int i = 0; i < nb_visible_lines; i++) {
    line_surfaces[i]->draw(dst_surface);
  }
}

}

