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
package org.solarus.editor;

import java.util.*;

/**
 * This class lists all elements of a certain resource.
 * A Resource object contain the maps, another one contains the musics, etc...
 */
public class Resource extends Observable {

    /**
     * Id and human-readable name of each element.
     */
    private LinkedHashMap<String, String> elements;

    /**
     * Creates the resource.
     */
    public Resource() {

        this.elements = new LinkedHashMap<String, String>();
    }

    /**
     * Returns an iterator over the resource ids.
     * @return an iterator over the ids
     */
    public Iterator<String> iterator() {
        return elements.keySet().iterator();
    }

    /**
     * Returns an array with the id of all elements.
     * @return an array with the id of all elements.
     */
    public String[] getIds() {

        String[] ids = new String[elements.size()];
        int i = 0;

        for (String id: elements.keySet()) {
            ids[i++] = id;
        }

        return ids;
    }

    /**
     * Returns whether an element exists.
     * @param id id of the element
     * @return true if it exists in the resource, false otherwise
     */
    public boolean exists(String id) {
        return elements.containsKey(id);
    }

    /**
     * Returns the name of an element from its id.
     * @param id id of an element
     * @return the name of this element
     * @throws QuestEditorException if this element doesn't exist
     */
    public String getElementName(String id) throws QuestEditorException {

        String name = elements.get(id);

        if (name == null) {
            throw new QuestEditorException("There is no element with id '" + id + "'");
        }

        return name;
    }

    /**
     * Changes the name of an element. If the element doesn't exist in the
     * database, it is added.
     * @param id id of an element
     * @param name the name of the element
     * @throws QuestEditorException if the id is not valid
     */
    public void setElementName(String id, String name) throws QuestEditorException {

        if (id.length() == 0) {
            throw new QuestEditorException("Empty id for element '" + name  + "'");
        }

        String oldName = elements.get(id);
        if (oldName == null || !name.equals(oldName)) {

            // the element doesn't exist yet, or its name has just been changed
            elements.put(id, name);
            setChanged();
            notifyObservers();
        }
    }
    /**
     * Add an element to the resource. 
     * If the element already exists in the database, throws an exception
     */
    public void addElement(String id, String name) throws QuestEditorException 
    {
        if (elements.get(id) == null) {
            elements.put(id,name);
            setChanged();
            notifyObservers();
        }
        else {
            throw new QuestEditorException("The element " + id + " already exists");
        }
    }
    /**
     * Removes an element from the resource.
     * Throws an exception if the element doesn't exist
     * in the database
     */
    public void removeElement(String id) throws QuestEditorException
    {
        if (elements.get(id) == null) {
            throw new QuestEditorException("The element " + id + " doesn't exist in the resource.");
        }
        elements.remove(id);
        setChanged();
        notifyObservers();
    }
    /**
     * Removes all elements from the resource.
     */
    public void clear() {
        elements.clear();
    }
}
