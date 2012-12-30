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
#include "GameoverSequence.h"
#include "Game.h"
#include "Savegame.h"
#include "Sprite.h"
#include "Map.h"
#include "Equipment.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "movements/TargetMovement.h"
#include <sstream>

/**
 * @brief Creates a game over sequence.
 * @param game the game
 * @param hero_direction direction of the hero sprite before game over
 */
GameoverSequence::GameoverSequence(Game& game, int hero_direction):
    game(game),
    music_id(Music::get_current_music_id()),
    gameover_menu_img("gameover_menu.png", Surface::DIR_LANGUAGE),
    hero_dead_sprite(get_tunic_animation()),
    fade_sprite("hud/gameover_fade"),
    fairy_sprite("entities/items"),
    fairy_movement(NULL),
    state(WAITING_START) {

  fade_sprite.stop_animation();
  red_screen_color = Color(224, 32, 32);

  hero_dead_sprite.set_current_animation("hurt");
  hero_dead_sprite.set_current_direction(hero_direction);
  hero_dead_sprite.set_suspended(true);

  const Rectangle &camera_position = game.get_current_map().get_camera_position();
  const Rectangle &hero_xy = game.get_hero_xy();
  hero_dead_x = hero_xy.get_x() - camera_position.get_x();
  hero_dead_y = hero_xy.get_y() - camera_position.get_y();

  fairy_sprite.set_current_animation("fairy");

  next_state_date = System::now() + 500;
}

/**
 * @brief Destroys the game over sequence.
 */
GameoverSequence::~GameoverSequence() {

  delete fairy_movement;
}

/**
 * @brief Updates the gameover sequence.
 */
void GameoverSequence::update() {

  uint32_t now = System::now();
  hero_dead_sprite.update();

  switch (state) {

    case WAITING_START:
      if (now >= next_state_date) {
        state = CLOSING_GAME;
        fade_sprite.restart_animation();
        Music::play(Music::none);
      }
      break;

    case CLOSING_GAME:
      fade_sprite.update();

      if (fade_sprite.is_animation_finished()) {
        state = RED_SCREEN;
        Sound::play("hero_dying");
        hero_dead_sprite.set_suspended(false);
        hero_dead_sprite.set_current_animation("dying");
        hero_dead_sprite.set_current_direction(0);
        next_state_date = now + 2000;
      }
      break;

    case RED_SCREEN:
      hero_dead_sprite.update();

      if (hero_dead_sprite.is_last_frame_reached() && now >= next_state_date) {
        state = OPENING_MENU;
        fade_sprite.set_current_animation("open");
      }
      break;

    case OPENING_MENU:
      fade_sprite.update();
      if (fade_sprite.is_animation_finished()) {

        Equipment &equipment = game.get_equipment();
        if (equipment.has_ability("get_back_from_death")) {
          state = SAVED_BY_FAIRY;
          fairy_x = hero_dead_x + 12;
          fairy_y = hero_dead_y + 21;
          fairy_movement = new TargetMovement(240, 22, 96, true);
          fairy_movement->set_xy(fairy_x, fairy_y);
          equipment.notify_ability_used("get_back_from_death");
        }
        else {
          state = MENU;
          Music::play("game_over");
          fairy_x = 76;
          fairy_y = 124;
          cursor_position = 0;
        }
      }
      break;

    case SAVED_BY_FAIRY:
      fairy_sprite.update();
      fairy_movement->update();
      fairy_x = fairy_movement->get_x();
      fairy_y = fairy_movement->get_y();

      if (fairy_movement->is_finished()) {
        state = WAITING_END;
        next_state_date = now + 1000;
        game.get_equipment().add_life(7 * 4);
      }
      break;

    case WAITING_END:
      if (now >= next_state_date) {
        state = RESUME_GAME;
        game.get_back_from_death();
        Music::play(music_id);
      }
      break;

    case RESUME_GAME:

      break;

    case MENU:
      fairy_sprite.update();
      break;

  }
}

/**
 * @brief Draws the gameover sequence.
 * @param dst_surface the surface to draw
 */
void GameoverSequence::draw(Surface& dst_surface) {

  if (state > CLOSING_GAME) {
    dst_surface.fill_with_color(Color::get_black());
  }

  if (state <= OPENING_MENU) {

    if (state == RED_SCREEN) {
      dst_surface.fill_with_color(red_screen_color);
    }
    else {
      fade_sprite.draw(dst_surface, hero_dead_x, hero_dead_y);
    }
  }

  if (state <= WAITING_END) {
    hero_dead_sprite.draw(dst_surface, hero_dead_x, hero_dead_y);

    if (state == SAVED_BY_FAIRY) {
      fairy_sprite.draw(dst_surface, fairy_x, fairy_y);
    }
  }
  else if (state == MENU) {
    gameover_menu_img.draw(dst_surface);
    fairy_sprite.draw(dst_surface, fairy_x, fairy_y);
  }
}

/**
 * @brief This function is called when a game command is pressed.
 * @param command The game command pressed.
 */
void GameoverSequence::notify_command_pressed(GameCommands::Command command) {

  if (state == MENU) {

    if (command == GameCommands::DOWN) {
      Sound::play("cursor");
      cursor_position = (cursor_position + 1) % 4;
      fairy_y = 124 + cursor_position * 16;
    }
    else if (command == GameCommands::UP) {
      Sound::play("cursor");
      cursor_position = (cursor_position + 3) % 4;
      fairy_y = 124 + cursor_position * 16;
    }
    else if (command == GameCommands::ACTION || command == GameCommands::ATTACK) {
      Sound::play("danger");
      game.get_equipment().add_life(7 * 4);

      switch (cursor_position) {

        case 0: // save and continue
          game.get_savegame().save();
          game.restart();
          break;

        case 1: // save and quit
          game.get_savegame().save();
          game.reset();
          break;

        case 2: // continue without saving
          game.restart();
          break;

        case 3: // quit without saving
          game.reset();
          break;
      }
    }
  }
}

/**
 * @brief Returns whether the game over sequence is finished.
 * @return true if the game over sequence is finished
 */
bool GameoverSequence::is_finished() {
  return state == RESUME_GAME;
}

/**
 * @brief Returns the animation set id of the hero's tunic sprite.
 * @return the animation set id of the tunic
 */
const std::string GameoverSequence::get_tunic_animation() {

  std::ostringstream oss;
  oss << "hero/tunic" << game.get_equipment().get_ability("tunic");
  return oss.str();
}
