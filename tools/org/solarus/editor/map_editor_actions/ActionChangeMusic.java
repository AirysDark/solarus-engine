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
package org.solarus.editor.map_editor_actions;

import org.solarus.editor.*;

/**
 * Changing the background music of the map.
 */
public class ActionChangeMusic extends MapEditorAction {

    private String musicBefore;
    private String musicAfter;

    /**
     * Constructor.
     * @param map the map
     * @param music the name of the music, i.e. a music file name,
     * Map.musicNone or Map.musicUnchanged
     */
    public ActionChangeMusic(Map map, String music) {
        super(map);

        this.musicBefore = map.getMusic();
        this.musicAfter = music;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
        map.setMusic(musicAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
        map.setMusic(musicBefore);
    }

}
