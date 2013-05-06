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
#include "TransitionScrolling.h"
#include "Game.h"
#include "Map.h"
#include "lowlevel/Color.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Debug.h"

/**
 * @brief Position where the previous map is blitted on both_maps_surface,
 * for each possible scrolling direction.
 */
const Rectangle TransitionScrolling::previous_map_dst_positions[] = {
  Rectangle(0,   0),  // scroll to the east
  Rectangle(0, SOLARUS_SCREEN_HEIGHT),  // scroll to the north
  Rectangle(SOLARUS_SCREEN_WIDTH,  0), // scroll to the west
  Rectangle(0 ,  0),  // scroll to the south
};

/**
 * @brief Creates a scrolling transition effect.
 * @param direction direction of the transition (in or out)
 */
TransitionScrolling::TransitionScrolling(Transition::Direction direction):
  Transition(direction), dx(0), dy(0) {

}

/**
 * @brief Destructor.
 */
TransitionScrolling::~TransitionScrolling() {

  if (direction == IN) {
    delete both_maps_surface;
  }
}

/**
 * @brief Starts this transition effect.
 */
void TransitionScrolling::start() {
  
  if (direction == OUT) {
    return;
  }

  // get the scrolling direction
  scrolling_direction = (game->get_current_map().get_destination_side() + 2) % 4;

  const int scrolling_step = 5;
  int width = SOLARUS_SCREEN_WIDTH;
  int height = SOLARUS_SCREEN_HEIGHT;
  if (scrolling_direction % 2 == 0) {
    // right or left
    width *= 2;
    dx = (scrolling_direction == 0) ? scrolling_step : -scrolling_step;
  }
  else {
    height *= 2;
    dy = (scrolling_direction == 3) ? scrolling_step : -scrolling_step;
  }

  // create a surface with the two maps
  both_maps_surface = new Surface(width, height);

  // set the blitting rectangles

  previous_map_dst_position = previous_map_dst_positions[scrolling_direction];
  current_map_dst_position = previous_map_dst_positions[(scrolling_direction + 2) % 4];
  current_scrolling_position = previous_map_dst_position;

  current_scrolling_position.set_size(SOLARUS_SCREEN_WIDTH, SOLARUS_SCREEN_HEIGHT);

  next_scroll_date = System::now();
}

/**
 * @brief Returns whether this transition effect needs the previous surface.
 * @return true
 */
bool TransitionScrolling::needs_previous_surface() {
  return true;
}

/**
 * @brief Returns whether the transition effect is started.
 * @return false
 */
bool TransitionScrolling::is_started() {
  return !is_finished();
}

/**
 * @brief Returns whether the transition effect is finished.
 * @return true
 */
bool TransitionScrolling::is_finished() {

  if (direction == OUT) {
    return true;
  }

  return current_scrolling_position.get_x() == current_map_dst_position.get_x()
    && current_scrolling_position.get_y() == current_map_dst_position.get_y();
}

/**
 * @brief Makes a scrolling step.
 */
void TransitionScrolling::scroll() {
  current_scrolling_position.add_xy(dx, dy);
}

/**
 * @brief Updates this transition effect.
 *
 * This function is called repeatedly while the transition exists.
 */
void TransitionScrolling::update() {

  if (!is_started()) {
    return;
  }

  uint32_t now = System::now();
  while (now >= next_scroll_date && !is_finished()) {
    scroll();
    next_scroll_date += 10;
  }
}

/**
 * @brief Draws the transition effect on a surface.
 * @param dst_surface the surface to draw
 */
void TransitionScrolling::draw(Surface& dst_surface) {

  if (direction == OUT) {
    return;
  }

  Debug::check_assertion(previous_surface != NULL,
      "No previous surface defined for scrolling");

  // draw the old map
  previous_surface->draw(*both_maps_surface, previous_map_dst_position);

  // draw the new map
  dst_surface.draw(*both_maps_surface, current_map_dst_position);

  // blit both surfaces
  dst_surface.fill_with_color(Color::get_black());
  both_maps_surface->draw_region(current_scrolling_position, dst_surface);
}

