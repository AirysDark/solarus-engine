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
#ifndef SOLARUS_COUNTER_H
#define SOLARUS_COUNTER_H

#include "Common.h"
#include "lowlevel/Rectangle.h"
#include "lowlevel/Surface.h"

/**
 * @brief Draws a counter on a surface.
 *
 * The counter can only draw positive integer numbers.
 */
class Counter {

  public:

    /**
     * Style of digits to display.
     */
    enum Style {
      BIG_DIGITS,   /**< 8*8 pixels (default) */
      SMALL_DIGITS  /**< 6*7 pixels */
    };

  private:

    Style style;
    unsigned int nb_digits;        /**< number of digits to use to display the value */
    bool fill_with_zeros;          /**< indicates to fill the counter with zeros when the number of digits is too low */
    unsigned int maximum;          /**< the counter is shown with a special color when
				    * this value is reached (0 indicates that there is no maximum) */
    unsigned int value;            /**< current value of the counter */

    Surface surface_drawn;         /**< intermediate surface where the counter is drawn */
    Rectangle dst_position;        /**< destination position when drawing the counter */
    Surface img_digits;            /**< source surface with digits */

    void rebuild_with_value(unsigned int value);

  public:

    Counter(unsigned int nb_digits, bool fill_with_zeros, int x, int y);
    ~Counter();

    void set_style(Style style);
    void set_maximum(unsigned int maximum);
    unsigned int get_value();
    void set_value(unsigned int value);
    void increase();
    void decrease();

    void draw(Surface& dst_surface);
    void draw(Surface& dst_surface, int x, int y);
};

#endif

