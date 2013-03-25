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
package org.solarus.editor.entities;

import java.util.NoSuchElementException;

import org.solarus.editor.*;

/**
 * A switch is a button that can be enabled by the hero or a block.
 */
public class Switch extends MapEntity {

    /**
     * Subtypes of switches.
     */
    public enum Subtype implements EntitySubtype {
        // We use integers ids for historical reasons.
        WALKABLE_INVISIBLE("0"),
        WALKABLE_VISIBLE("1"),
        ARROW_TARGET("2"),
        SOLID("3");

        public static final String[] humanNames = {
          "Walkable invisible",
          "Walkable visible",
          "Arrow target",
          "Solid"
        };

        private String id;

        private Subtype(String id) {
            this.id = id;
        }

        public String getId() {
            return id;
        }

        public static Subtype get(String id) {
            for (Subtype subtype: values()) {
                if (subtype.getId().equals(id)) {
                    return subtype;
                }
            }
            throw new NoSuchElementException(
                    "No crystal block subtype with id '" + id + "'");
        }

        public static boolean isWalkable(EntitySubtype subtype) {
            return subtype == WALKABLE_INVISIBLE || subtype == WALKABLE_VISIBLE;
        }
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("miscellaneous_entities.png", 56, 16, 16, 16),  // walkable invisible
        new EntityImageDescription("miscellaneous_entities.png", 72, 16, 16, 16),  // walkable visible
        new EntityImageDescription("miscellaneous_entities.png", 88, 16, 16, 16),  // arrow target
        new EntityImageDescription("miscellaneous_entities.png", 104, 16, 16, 16), // solid
    };

    /**
     * Creates a new switch.
     * @param map the map
     */
    public Switch(Map map) throws MapException {
        super(map, 16, 16);
    }

    /**
     * Returns whether this switch is walkable.
     * @return true if the subtype is WALKABLE_INVISIBLE or WALKABLE_VISIBLE
     */
    public boolean isWalkable() {
        return Subtype.isWalkable(getSubtype());
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     * @throws MapException if the subtype is not valid
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {

        if (subtype != this.subtype) {
            if (subtype == Subtype.WALKABLE_INVISIBLE) {
                setBooleanProperty("needs_block", false);
            }
            else if (Subtype.isWalkable(subtype)) {
                setBooleanProperty("needs_block", false);
                setBooleanProperty("inactivate_when_leaving", false);
            }
            super.setSubtype(subtype);
        }
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
        setBooleanProperty("needs_block", false);
        setBooleanProperty("inactivate_when_leaving", false);
        setSubtype(Subtype.WALKABLE_INVISIBLE);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

        if (getSubtype() == Subtype.WALKABLE_INVISIBLE && getBooleanProperty("needs_block")) {
            throw new MapException("Cannot put a block on an invisible switch");
        }

        if (!isWalkable()) {

            if (getBooleanProperty("needs_block")) {
                throw new MapException("Cannot put a block on a non-walkable switch");
            }

            if (getBooleanProperty("inactivate_when_leaving")) {
                throw new MapException("Cannot disable the switch when leaving for a non-walkable switch");
            }
        }
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
        currentImageDescription = generalImageDescriptions[subtype.ordinal()];
    }
}

