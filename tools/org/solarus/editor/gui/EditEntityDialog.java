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
import org.solarus.editor.entities.*;

/**
 * A dialog box with some options to edit a map entity.
 */
public class EditEntityDialog extends OkCancelDialog {

    /**
     * The component to edit the entity.
     */
    private EditEntityComponent entityComponent;

    /**
     * Constructor.
     */
    public EditEntityDialog(Map map, MapEntity entity) {
        super("Edit an entity", false);
        entityComponent = EditEntityComponent.create(map, entity);
        setComponent(entityComponent);
    }

    /**
     * Takes into account the modifications made by the user in the dialog box.
     * @throws QuestEditorException if the user has made incorrect modifications
     */
    protected void applyModifications() throws QuestEditorException {
        entityComponent.applyModifications();
    }
}
