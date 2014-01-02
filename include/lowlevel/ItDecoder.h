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
#ifndef SOLARUS_IT_DECODER_H
#define SOLARUS_IT_DECODER_H

#include "Common.h"
#include <modplug.h>
#include <cstddef>

namespace solarus {

/**
 * \brief Encapsulates the Impulse Tracker music decoding.
 *
 * This class allows the Music class to be independent of the Impulse Tracker decoding library.
 */
class ItDecoder {

  public:

    ItDecoder();
    ~ItDecoder();

    void load(void* sound_data, size_t sound_size);
    void unload();
    void decode(void* decoded_data, int nb_samples);

    int get_num_channels();
    int get_channel_volume(int channel);
    void set_channel_volume(int channel, int volume);
    int get_tempo();
    void set_tempo(int tempo);

  private:

    ModPlugFile* modplug_file;

};

}

#endif

