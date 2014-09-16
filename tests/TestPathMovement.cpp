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
/* TODO update this test
#include "Solarus.h"
#include "movements/PathMovement.h"
#include "Game.h"
#include "Savegame.h"
#include "Map.h"
#include "entities/MapEntities.h"
#include "entities/InteractiveEntity.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

static void syntax_test(MapEntity &e) {

  Game &game = e.get_game();

  bool no_error = false;
  try {

    PathMovement *movement = new PathMovement("abc", 100, false, false, false); // incorrect path
    e.set_movement(movement);
    while (!movement->is_finished()) {
      game.update();
      System::update();
    }

    no_error = true;
  }
  catch (std::logic_error &e) {
    // expected behavior
  }

  e.clear_movement();
  Debug::check_assertion(!no_error, "'syntax_test' failed to detect a syntax error");
}

static void one_step_test(MapEntity &e) {

  Game &game = e.get_game();
  Rectangle old_xy = e.get_xy();

  PathMovement *movement = new PathMovement("0", 100, false, false, false); // 8 pixels to the right
  e.set_movement(movement);

  while (!movement->is_finished()) {
    game.update();
    System::update();
  }

  Debug::check_assertion(e.get_x() - old_xy.get_x() == 8 && e.get_y() - old_xy.get_y() == 0,
      StringConcat() << "Unexcepted coordinates for 'one_step_test #1': " << e.get_xy());
  Debug::check_assertion(movement->get_total_distance_covered() == 8,
      StringConcat() << "Unexpected distance covered for 'one_step_test #1': " << movement->get_total_distance_covered());

  movement->set_path("4");
  while (!movement->is_finished()) {
    game.update();
    System::update();
  }
  Debug::check_assertion(e.get_x() == old_xy.get_x() && e.get_y() == old_xy.get_y(),
      StringConcat() << "Unexpected coordinates for 'one_step_test #2': " << e.get_xy());
  Debug::check_assertion(movement->get_total_distance_covered() == 16,
      StringConcat() << "Unexpected distance covered for 'one_step_test #2': " << movement->get_total_distance_covered());

  movement->set_path("3");
  while (!movement->is_finished()) {
    game.update();
    System::update();
  }
  Debug::check_assertion(e.get_x() - old_xy.get_x() == -8 && e.get_y() - old_xy.get_y() == -8,
      StringConcat() << "Unexpected coordinates for 'one_step_test #3': " << e.get_xy());
  Debug::check_assertion(movement->get_total_distance_covered() == 24,
      StringConcat() << "Unexpected distance covered for 'one_step_test #3': " << movement->get_total_distance_covered());

  movement->set_path("7");
  while (!movement->is_finished()) {
    game.update();
    System::update();
  }
  Debug::check_assertion(e.get_x() == old_xy.get_x() && e.get_y() == old_xy.get_y(),
      StringConcat() << "Unexpected coordinates for 'one_step_test #4': " << e.get_xy());
  Debug::check_assertion(movement->get_total_distance_covered() == 32,
      StringConcat() << "Unexpected distance covered for 'one_step_test #4': " << movement->get_total_distance_covered());

  e.clear_movement();
}

static void direction_test(MapEntity &e) {

  Game &game = e.get_game();
  Rectangle old_xy = e.get_xy();

  PathMovement *movement = new PathMovement("5", 100, false, false, false); // 8 pixels to the right
  e.set_movement(movement);

  Debug::check_assertion(movement->get_current_direction() == 5,
      StringConcat() << "Unexcepted current direction for 'direction_test #1': " << movement->get_current_direction());

  while (!movement->is_finished()) {
    game.update();
    System::update();
  }

  // when the movement is finished, PathMovement::get_current_direction() must return the last direction
  Debug::check_assertion(movement->get_current_direction() == 5,
      StringConcat() << "Unexcepted last direction for 'direction_test #1': " << movement->get_current_direction());
}

static void multi_step_test(MapEntity &e) {

  Game &game = e.get_game();
  Rectangle old_xy = e.get_xy();

  PathMovement *movement = new PathMovement("66", 100, false, false, false); // 16 pixels downwards
  e.set_movement(movement);

  while (!movement->is_finished()) {
    game.update();
    System::update();
  }

  Debug::check_assertion(e.get_x() - old_xy.get_x() == 0 && e.get_y() - old_xy.get_y() == 16,
      StringConcat() << "Unexcepted coordinates for 'multi_step_test #1': " << e.get_xy());
  Debug::check_assertion(movement->get_total_distance_covered() == 16,
      StringConcat() << "Unexpected distance covered for 'multi_step_test #1': " << movement->get_total_distance_covered());

  movement->set_path("220");

  while (!movement->is_finished()) {
    game.update();
    System::update();
  }

  Debug::check_assertion(e.get_x() - old_xy.get_x() == 8 && e.get_y() - old_xy.get_y() == 0,
      StringConcat() << "Unexcepted coordinates for 'multi_step_test #2': " << e.get_xy());
  Debug::check_assertion(movement->get_total_distance_covered() == 40,
      StringConcat() << "Unexpected distance covered for 'multi_step_test #2': " << movement->get_total_distance_covered());

  e.clear_movement();
}

static void snap_test(MapEntity &e) {

  Game &game = e.get_game();

  e.set_top_left_xy(155, 108); // not aligned to the grid

  // #1: a path movement that does not require the entity to snap to the grid
  PathMovement *movement = new PathMovement("56", 100, false, false, false);
  e.set_movement(movement);
  while (!movement->is_finished()) {
    game.update();
    System::update();
  }
  Debug::check_assertion(e.get_top_left_x() == 147 && e.get_top_left_y() == 124,
      StringConcat() << "Unexcepted coordinates for 'snap_test #1': " << e.get_xy());
  Debug::check_assertion(movement->get_total_distance_covered() == 16,
      StringConcat() << "Unexpected distance covered for 'snap_test #1': " << movement->get_total_distance_covered());
  e.clear_movement();

  // #2: a path movement that requires the entity to snap to the grid
  movement = new PathMovement("21", 100, false, false, true);
  e.set_movement(movement);
  while (!movement->is_finished()) {
    game.update();
    System::update();
  }
  Debug::check_assertion(e.get_top_left_x() == 152 && e.get_top_left_y() == 112,
      StringConcat() << "Unexcepted coordinates for 'snap_test #2': " << e.get_xy());
  Debug::check_assertion(movement->get_total_distance_covered() == 16,
      StringConcat() << "Unexpected distance covered for 'snap_test #2': " << movement->get_total_distance_covered());


}
*/

/*
 * Test for the path movement.
 */
int main(int /* argc */, char** /* argv */) {

  /*
  Solarus solarus(argc, argv);
  Savegame savegame("save_initial.dat");
  Game game(solarus, savegame);
  game.update();

  Map &map = game.get_current_map();
  MapEntity *e = new InteractiveEntity(game, "e", LAYER_LOW, 160, 117,
      InteractiveEntity::CUSTOM, "npc/sahasrahla", 0, "_none");
  map.get_entities().add_entity(e);

  syntax_test(*e);
  one_step_test(*e);
  multi_step_test(*e);
  direction_test(*e);
  snap_test(*e);
*/
  return 0;
}

