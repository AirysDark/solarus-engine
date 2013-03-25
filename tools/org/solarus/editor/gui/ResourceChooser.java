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

import java.util.*;
import javax.swing.*;
import org.solarus.editor.*;

/**
 * A combo box component to select an element from the resource database.
 */
public class ResourceChooser extends JComboBox<KeyValue> implements ProjectObserver {

    /**
     * The kind of resource displayed in the combo box (maps, musics...).
     */
    private ResourceType resourceType;

    /**
     * A list of special options to add to the combo box.
     */
    private KeyValue[] additionalOptions;

    /**
     * Constructor.
     * @param resourceType the kind of resource displayed in the combo box
     * @param showEmptyOption true to include an empty option in the combo box
     */
    public ResourceChooser(ResourceType resourceType, boolean showEmptyOption) {

        super();
        this.resourceType = resourceType;

        if (showEmptyOption) {
            this.additionalOptions = new KeyValue[] {new KeyValue("", "")};
        }
        else {
            this.additionalOptions = new KeyValue[] {};
        }

        Project.addProjectObserver(this);

        buildList();
    }

    /**
     * Sets some additional options to add at the beginning of the combo box
     * @param additionalOptions a list of special options to add to the combo box
     */
    protected void setAdditionalOptions(KeyValue[] additionalOptions) {

        this.additionalOptions = additionalOptions;
        reloadList();
    }

    /**
     * Loads the list using the game resource database.
     */
    protected void buildList() {

        if (!Project.isLoaded()) {
            return;
        }

        for (int i = 0; i < additionalOptions.length; i++) {
            addItem(additionalOptions[i]);
        }

        try {
            Resource resource = Project.getResource(resourceType);
            String[] ids = resource.getIds();
            String name;

            for (int i = 0; i < ids.length; i++) {
                name = resource.getElementName(ids[i]);
                addItem(new KeyValue(ids[i], name));
            }
        }
        catch (QuestEditorException ex) {
            System.err.println("Unexpected error: " + ex.getMessage());
            ex.printStackTrace();
            System.exit(1);
        }
    }

    /**
     * Reloads the list using the game resource database.
     * After the list is reloaded, the same item is selected again if it still exists.
     */
    protected void reloadList() {

        String selectedId = getSelectedId();

        removeAllItems();
        buildList();

        if (selectedId.length() > 0) {
            try {
                setSelectedId(selectedId);
            }
            catch (NoSuchElementException ex) {
                // The element no longer exists, nevermind.
            }
        }
    }

    /**
     * Returns the id of the currently selected element.
     * @return the id of the selected element, or an empty string if no element is selected
     */
    public String getSelectedId() {

        KeyValue item = (KeyValue) getSelectedItem();

        if (item == null) {
            return "";
        }

        return item.getKey();
    }

    /**
     * Changes the currently selected element.
     * If the specified element doesn't exist, a NoSuchElementException is raised.
     * @param id id of the element you want to make selected in the combo box,
     * or an empty string to select no element
     * @throws NoSuchElementException if the id specified does not exist in the combo box
     */
    public void setSelectedId(String id) throws NoSuchElementException {

        KeyValue item = new KeyValue(id, null);
        setSelectedItem(item);

        if (!getSelectedId().equals(id)) {
          throw new NoSuchElementException("No id '" + id + "' in the list");
        }
    }

    /**
     * This method is called when a project has just been loaded.
     * The resource list is then loaded.
     */
    public void currentProjectChanged() {
        reloadList();
    }
}
