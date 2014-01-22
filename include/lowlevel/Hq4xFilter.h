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
#ifndef SOLARUS_HQ4X_FILTER_H
#define SOLARUS_HQ4X_FILTER_H

#include "Common.h"
#include "lowlevel/PixelFilter.h"

namespace solarus {

/**
 * \brief Wrapper to the hq4x algorithm.
 */
class Hq4xFilter: public PixelFilter {

  public:

    Hq4xFilter();
    ~Hq4xFilter();

    int get_scaling_factor() const;
    void filter(
        const uint32_t* src,
        int src_width,
        int src_height,
        uint32_t* dst) const;

    static void initialize_hqx();

};

}

#endif

