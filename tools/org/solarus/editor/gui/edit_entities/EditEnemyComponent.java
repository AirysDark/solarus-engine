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
package org.solarus.editor.gui.edit_entities;

import java.awt.event.*;
import javax.swing.*;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.entities.Enemy.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit an enemy.
 */
public class EditEnemyComponent extends EditEntityComponent {

    // specific fields
    private ResourceChooser breedField;
    private EnumerationChooser<Enemy.Rank> rankField;
    private JCheckBox saveField;
    private JTextField savegameVariableField;
    private TreasureChooser treasureField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditEnemyComponent(Map map, MapEntity entity) {
        super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

        // breed
        breedField = new ResourceChooser(ResourceType.ENEMY, true);
        addField("Breed", breedField);

        // rank
        rankField = new EnumerationChooser<Rank>(Rank.class);
        addField("Rank", rankField);

        // save the enemy or not
        saveField = new JCheckBox("Save the enemy state");
        addField("Savegame", saveField);

        // savegame variable
        savegameVariableField = new JTextField(20);
        addField("Enemy savegame variable", savegameVariableField);

        // treasure
        treasureField = new TreasureChooser(true);
        addField("Treasure", treasureField);

        // enable or disable the 'savegame variable' field depending on the checkbox
        saveField.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ev) {
            savegameVariableField.setEnabled(saveField.isSelected());
          }
        });
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
        super.update(); // update the common fields

        Enemy enemy = (Enemy) entity;

        breedField.setSelectedId(enemy.getProperty("breed"));
        rankField.setValue(Rank.get(enemy.getIntegerProperty("rank")));

        String savegameVariable = enemy.getProperty("savegame_variable");
        if (savegameVariable != null) {
          savegameVariableField.setText(savegameVariable);
          savegameVariableField.setEnabled(true);
          saveField.setSelected(true);
        }
        else {
          savegameVariableField.setEnabled(false);
          saveField.setSelected(false);
        }

        treasureField.setTreasure(
                enemy.getProperty("treasure_name"),
                enemy.getIntegerProperty("treasure_variant"),
                enemy.getProperty("treasure_savegame_variable"));
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

        String savegameVariable = savegameVariableField.isEnabled() ?
                savegameVariableField.getText() : null;
        String treasureName = treasureField.getTreasure().getItemName();
        Integer treasureVariant = treasureField.getTreasure().getVariant();
        String treasureSavegameVariable = treasureField.getTreasure().getSavegameVariable();

        return new ActionEditEntitySpecific(entity,
                breedField.getSelectedId(),
                Integer.toString(rankField.getValue().getId()),
                savegameVariable,
                treasureName,
                treasureVariant == null ? null : Integer.toString(treasureVariant),
                treasureSavegameVariable);
    }
}

