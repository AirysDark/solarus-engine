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

/**
 * Interface for the subtype enumeration of an entity type.
 * Every enumeration implementing this interface must also define:
 * - the static method public static Subtype get(int id)
 *   which converts an integer into a value of the enumeration, and
 * - the static field public static final String[] humanNames
 *   which define a human readable name for each element of the enumeration.
 * The value with id zero is considered as the default value when a new entity
 * is created.
 */
public interface EntitySubtype {

    /**
     * Returns the id corresponding to the enumeration value.
     * @return the id
     */
    public String getId();

    /**
     * Returns the index of this value in the enumeration.
     * @return the index
     * TODO kill this function, it's too dangerous to rely on this
     */
    public int ordinal();
}
