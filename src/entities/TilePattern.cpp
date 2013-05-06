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
#include "entities/TilePattern.h"
#include "entities/AnimatedTilePattern.h"
#include "entities/TimeScrollingTilePattern.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Surface.h"

/**
 * @brief Constructor.
 *
 * It is called by the subclasses. 
 * 
 * @param obstacle type of obstacle
 * @param width width of the tile in pixels (must be a multiple of 8)
 * @param height height of the tile in pixels (must be a multiple of 8)
 */
TilePattern::TilePattern(Obstacle obstacle, int width, int height):
  obstacle(obstacle), width(width), height(height) {

  // check the width and the height
  Debug::check_assertion(width > 0
      && height > 0
      && width % 8 == 0
      && height % 8 == 0,
      StringConcat() << "Invalid tile pattern: the size is (" << width << "x" << height <<
      ") but should be positive and multiple of 8 pixels");

  // diagonal obstacle: check that the tile is square
  Debug::check_assertion(obstacle < OBSTACLE_TOP_RIGHT
      || obstacle > OBSTACLE_BOTTOM_RIGHT
      || width == height,
      "Invalid tile pattern: a tile pattern with a diagonal obstacle must be square");
}

/**
 * @brief Destructor.
 */
TilePattern::~TilePattern() {

}

/**
 * @brief Returns the width of the tile pattern.
 * @return the width of the tile 
 */
int TilePattern::get_width() const {
  return width;
}

/**
 * @brief Returns the height of the tile pattern.
 * @return the height of the tile 
 */
int TilePattern::get_height() const {
  return height;
}

/**
 * @brief Returns the obstacle property of this tile pattern.
 * @return the obstacle property of this tile pattern
 */
Obstacle TilePattern::get_obstacle() const {
  return obstacle;
}

/**
 * @brief Updates the current frame of all tile patterns.
 *
 * This function is called repeatedly by the map.
 */
void TilePattern::update() {
  AnimatedTilePattern::update();
  TimeScrollingTilePattern::update();
}

/**
 * @brief Returns whether this tile pattern is animated, i.e. not always drawn
 * the same way.
 *
 * Non-animated tiles may be rendered faster by using intermediate surfaces
 * that are drawn only once.
 * This function should return false if the tile pattern is always drawn the same way.
 * Returns true by default.
 *
 * @return true if this tile pattern is animated
 */
bool TilePattern::is_animated() {
  return true;
}

/**
 * @brief Returns whether tiles having this tile pattern are drawn at their
 * position.
 *
 * Usually, this function returns true, and when it is the case, draw() is
 * called only for tiles that are located in the current viewport.
 *
 * However, some tile patterns may want to be drawn even when they are not
 * in the viewport, typically to make an illusion of movement like parallax
 * scrolling.
 *
 * @return true if tiles having this pattern are drawn where they are placed
 */
bool TilePattern::is_drawn_at_its_position() {
  return true;
}

/**
 * @brief Fills a rectangle by repeating this tile pattern.
 * @param dst_surface The destination surface.
 * @param dst_position Coordinates of the rectangle to fill in \c dst_surface.
 * @param tileset The tileset to use.
 * @param viewport Coordinates of the top-left corner of \c dst_surface
 * relative to the map (may be used for scrolling tiles).
 */
void TilePattern::fill_surface(Surface& dst_surface, const Rectangle& dst_position,
    Tileset& tileset, const Rectangle& viewport) {

  Rectangle dst(0, 0);

  int limit_x = dst_position.get_x() + dst_position.get_width();
  int limit_y = dst_position.get_y() + dst_position.get_height();

  for (int y = dst_position.get_y();
      y < limit_y;
      y += get_height()) {

    if ((y <= dst_surface.get_height() && y + get_height() > 0)
        || !is_drawn_at_its_position()) {
      dst.set_y(y);

      for (int x = dst_position.get_x();
          x < limit_x;
          x += get_width()) {

        if ((x <= dst_surface.get_width() && x + get_width() > 0)
            || !is_drawn_at_its_position()) {
          dst.set_x(x);
          draw(dst_surface, dst, tileset, viewport);
        }
      }
    }
  }
}

