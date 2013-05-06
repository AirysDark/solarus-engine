/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.gui;

import org.solarus.editor.*;

/**
 * A combo box component to select a music from the resource database.
 */
public class MusicChooser extends ResourceChooser {

    /**
     * Constructor.
     */
    public MusicChooser() {
        super(ResourceType.MUSIC, false);
    }

    /**
     * Rebuils the list. Two special elements (no music and no change)
     * are added at the beginning of the list.
     */
    protected void buildList() {

        addItem(new KeyValue(Music.noneId, Music.noneName));
        addItem(new KeyValue(Music.unchangedId, Music.unchangedName));

        super.buildList();
    }
}
