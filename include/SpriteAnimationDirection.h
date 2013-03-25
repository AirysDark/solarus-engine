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
#ifndef SOLARUS_SPRITE_ANIMATION_DIRECTION
#define SOLARUS_SPRITE_ANIMATION_DIRECTION

#include "Common.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief A sequence of frames representing a sprite animated in a particular direction.
 *
 * An animation direction is a sequence of frames representing a sprite
 * with a specific animation and in a specific direction.
 */
class SpriteAnimationDirection {

  private:

    const int nb_frames;    /**< number of frames in this sequence */
    Rectangle* frames;      /**< position of each frame of the sequence on the image */
    Rectangle origin;       /**< coordinates of the sprite's origin from the
                             * upper-left corner of its image. */

    PixelBits** pixel_bits; /**< bit masks representing the non-transparent pixels of each frame,
                             * computed only if enable_pixel_collisions() is called */

  public:

    // creation and destruction
    SpriteAnimationDirection(int nb_frames, Rectangle* frames,
        int x_origin, int y_origin);
    ~SpriteAnimationDirection();

    // size and origin point
    Rectangle get_size() const;
    const Rectangle& get_origin() const;

    // frames
    int get_nb_frames() const;
    const Rectangle& get_frame(int frame) const;
    void draw(Surface& dst_surface, const Rectangle& dst_position,
        int current_frame, Surface& src_image);

    // pixel collisions
    void enable_pixel_collisions(Surface* src_image);
    void disable_pixel_collisions();
    bool are_pixel_collisions_enabled() const;
    PixelBits& get_pixel_bits(int frame) const;
};

#endif

