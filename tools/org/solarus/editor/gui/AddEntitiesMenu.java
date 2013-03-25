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

import java.awt.event.*;
import javax.swing.*;
import org.solarus.editor.entities.*;

/**
 * A menu with items to create any kind of entity.
 */
public class AddEntitiesMenu extends JMenu {

    /**
     * All menu items.
     */
    private static final ItemDefinition[] itemDefinitions = {
        new ItemDefinition(EntityType.DESTINATION, KeyEvent.VK_D),
        new ItemDefinition(EntityType.TELETRANSPORTER, KeyEvent.VK_T),
        new ItemDefinition(EntityType.PICKABLE, KeyEvent.VK_P),
        new ItemDefinition(EntityType.DESTRUCTIBLE, KeyEvent.VK_M,
                Destructible.Subtype.GRASS,
                Destructible.Subtype.BUSH,
                Destructible.Subtype.POT,
                Destructible.Subtype.STONE_SMALL_WHITE,
                Destructible.Subtype.STONE_SMALL_BLACK,
                Destructible.Subtype.BOMB_FLOWER
        ),
        new ItemDefinition(EntityType.CHEST, KeyEvent.VK_C),
        new ItemDefinition(EntityType.ENEMY, KeyEvent.VK_E),
        new ItemDefinition(EntityType.NPC, KeyEvent.VK_I,
                NPC.Subtype.GENERALIZED_NPC,
                NPC.Subtype.USUAL_NPC
        ),
        new ItemDefinition(EntityType.BLOCK, KeyEvent.VK_B),
        new ItemDefinition(EntityType.SWITCH, KeyEvent.VK_W,
                Switch.Subtype.WALKABLE_INVISIBLE,
                Switch.Subtype.WALKABLE_VISIBLE,
                Switch.Subtype.ARROW_TARGET
        ),
        new ItemDefinition(EntityType.WALL, KeyEvent.VK_O),
        new ItemDefinition(EntityType.SENSOR, KeyEvent.VK_N),
        new ItemDefinition(EntityType.CRYSTAL, KeyEvent.VK_Y),
        new ItemDefinition(EntityType.CRYSTAL_BLOCK, KeyEvent.VK_N,
                CrystalBlock.Subtype.ORANGE,
                CrystalBlock.Subtype.BLUE
        ),
        new ItemDefinition(EntityType.SHOP_ITEM, KeyEvent.VK_H),
        new ItemDefinition(EntityType.CONVEYOR_BELT, KeyEvent.VK_V),
        new ItemDefinition(EntityType.DOOR, KeyEvent.VK_R),
        new ItemDefinition(EntityType.STAIRS, KeyEvent.VK_S,
                CrystalBlock.Subtype.ORANGE,
                CrystalBlock.Subtype.BLUE
        ),
    };

    /**
     * Defines an item of the menu.
     */
    private static class ItemDefinition {
        private EntityType type;
        private EntitySubtype[] subtypes;
        private int key;

        /**
         * Creates an item.
         * @param type type of the entity to create
         * @param key a mnemonic key (like KeyEvent.VK_A)
         * @param subtypes subtypes to propose (none if there is no subtype)
         */
        public ItemDefinition(EntityType type, int key, EntitySubtype ... subtypes) {
            this.type = type;
            this.subtypes = subtypes;
            this.key = key;

            if (!hasSubtypes() && type.hasSubtype()) {
                System.err.println("Subtypes missing in entities menu for type " + type);
                System.exit(1);
            }
        }

        public EntityType getEntityType() {
            return type;
        }

        public boolean hasSubtypes() {
            return subtypes.length != 0;
        }

        public EntitySubtype[] getEntitySubtypes() {
            return subtypes;
        }

        public int getKey() {
            return key;
        }
    }

    /**
     * The map view.
     */
    private MapView mapView;

    /**
     * Constructor.
     * @param mapView the map view
     * @param title name of the menu
     */
    public AddEntitiesMenu(MapView mapView, String title) {
        super(title);
        this.mapView = mapView;

        JMenuItem item;
        for (ItemDefinition def: itemDefinitions) {
            EntityType type = def.getEntityType();
            if (!def.hasSubtypes()) {
                item = new JMenuItem(type.getHumanName());
                item.setMnemonic(def.getKey());
                item.addActionListener(new ActionListenerAddEntity(type));
            }
            else {
                item = new JMenu(type.getHumanName());
                item.setMnemonic(def.getKey());

                for (EntitySubtype subtype: def.getEntitySubtypes()) {
                    JMenuItem subitem = new JMenuItem(type.getSubtypeName(subtype));
                    subitem.addActionListener(new ActionListenerAddEntity(type, subtype));
                    item.add(subitem);
                }
            }
            add(item);
        }
    }

    /**
     * Action performed when the user wants to add an entity.
     */
    private class ActionListenerAddEntity implements ActionListener {

        // type of entity to add
        private EntityType entityType;
        private EntitySubtype entitySubtype;

        public ActionListenerAddEntity(EntityType entityType) {
            this(entityType, null);
        }

        public ActionListenerAddEntity(EntityType entityType, EntitySubtype entitySubtype) {
            this.entityType = entityType;
            this.entitySubtype = entitySubtype;
        }

        public void actionPerformed(ActionEvent ev) {
            mapView.startAddingEntity(entityType, entitySubtype);
        }
    }
}
