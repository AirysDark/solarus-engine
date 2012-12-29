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

import java.awt.*;
import java.util.Observable;
import javax.swing.*;

import org.solarus.editor.*;
import org.solarus.editor.entities.*;

/**
 * Main window of the tileset editor.
 */
public class TilesetEditorWindow extends AbstractEditorWindow implements ProjectObserver {

    /**
     * The current tileset.
     */
    private Tileset tileset;
    /**
     * The list of tile patterns.
     */
    private TilePatternsView tilePatternsView;
    /**
     * The tileset image.
     */
    private TilesetImageView tilesetImageView;
    // menus or menu items memorized to enable it later
//    private JMenu menuTileset;
//    private JMenuItem menuItemClose;
//    private JMenuItem menuItemSave;

    /**
     * Creates a new window.
     */
    public TilesetEditorWindow(String quest, EditorWindow parentEditor) {
        setLayout(new BorderLayout());
        Project.addProjectObserver(this);
        this.parentEditor = parentEditor;
        // set a nice look and feel
        GuiTools.setLookAndFeel();

        // create the menu bar
//        createMenuBar();

        // tile patterns list and tileset image

        // tile patterns list
        tilePatternsView = new TilePatternsView();
        tilePatternsView.setAlignmentY(Component.TOP_ALIGNMENT);
        tilePatternsView.setMaximumSize(new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));

        // tileset image
        tilesetImageView = new TilesetImageView(true);
        JScrollPane tilesetImageScroller = new JScrollPane(tilesetImageView);
        tilesetImageScroller.setAlignmentY(Component.TOP_ALIGNMENT);
        tilesetImageScroller.setMaximumSize(new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));

        JSplitPane tilesetPanel = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, tilePatternsView, tilesetImageScroller);
        tilesetPanel.setContinuousLayout(true);
        tilesetPanel.resetToPreferredSizes();
        // we must put our main panel in another panel
        // otherwise the background color of the window is bad
        add(tilesetPanel);
        //add(rootPanel);

//        // add a window listener to confirm when the user closes the window
//        setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
//        addWindowListener(new WindowAdapter() {
//                public void windowClosing(WindowEvent e) {
//                    if (checkCurrentFileSaved()) {
//                        dispose();
//                    }
//                }
//            });
//
//        loadProject();
    }

    public TilesetEditorWindow(String quest, EditorWindow editorWindow, Tileset t) {
        this(quest, editorWindow);
        setTileset(t);
    }

    /**
     * This method is called when a project has just been loaded.
     * The tileset menu is enabled.
     */
    public void currentProjectChanged() {
        //menuTileset.setEnabled(true);

        if (tileset != null) {
            closeTileset(); // close the tileset that was open with the previous project
        }
    }

    /**
     * Sets the current tileset. This method is called when the user opens a tileset,
     * closes the tileset, or creates a new one.
     * @param tileset the new tileset, or null if no tileset is loaded
     */
    private void setTileset(Tileset tileset) {
        // if there was already a tileset, remove its observers
        if (this.tileset != null) {
            this.tileset.deleteObservers();
        }

        this.tileset = tileset;

        tileset.addObserver(parentEditor);
        // enable or disable the menu items
//        menuItemClose.setEnabled(tileset != null);
//        menuItemSave.setEnabled(tileset != null);

        // notify the views
        tilePatternsView.setTileset(tileset);
        tilesetImageView.setTileset(tileset);
    }

    /**
     * This function is called when the tileset changes.
     * @param o the history
     * @param obj additional parameter
     */
    public void update(Observable o, Object obj) {
        this.parentEditor.update(o, obj);
    }

    /**
     * This function is called when the user wants to close the current tileset.
     * If the tileset is not saved, we propose to save it.
     * @return false if the user cancelled
     */
    public boolean checkCurrentFileSaved() {
        boolean result = true;

        if (tileset != null && !tileset.isSaved()) {
            int answer = JOptionPane.showConfirmDialog(this,
                    "The tileset has been modified. Do you want to save it?",
                    "Save the modifications",
                    JOptionPane.YES_NO_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            if (answer == JOptionPane.YES_OPTION) {
                saveTileset();
            } else if (answer == JOptionPane.CANCEL_OPTION) {
                result = false;
            }
        }

        return result;
    }

    /**
     * Give the name of the resource opened in the editor
     * @return the name of the map
     */
    public Tileset getTileset() {
        return tileset;
    }

    /**
     * Give the name of the resource opened in the editor
     * @return the name of the map
     */
    public String getResourceName() {
        return "Tileset " + getTileset().getName();
    }

    /**
     * Creates a new tileset in the project and sets it as the current tileset.
     */
    protected void newTileset() {

        if (!checkCurrentFileSaved()) {
            return;
        }

        try {
            String tilesetId = JOptionPane.showInputDialog(null, "Please enter the ID of your new tileset",
                    "tileset ID", JOptionPane.QUESTION_MESSAGE);

            if (tilesetId != null) {
                if (Project.getResource(ResourceType.TILESET).exists(tilesetId)) {
                    throw new MapException("A tileset already exists with the ID '" + tilesetId + "'");
                }

                Tileset tileset = new Tileset(tilesetId);
                setTileset(tileset);
            }
        }
        catch (QuestEditorException ex) {
            GuiTools.errorDialog("Cannot create the tileset: " + ex.getMessage());
        }
    }

    /**
     * Loads a tileset of the project and sets it as the current tileset.
     */
    protected void openTileset() {

        if (!checkCurrentFileSaved()) {
            return;
        }

        ResourceChooserDialog dialog = new ResourceChooserDialog(ResourceType.TILESET);
        dialog.setLocationRelativeTo(TilesetEditorWindow.this);
        dialog.pack();
        dialog.setVisible(true);
        String tilesetId = dialog.getSelectedId();

        if (tilesetId.length() == 0) {
            return;
        }

        try {
            if (!Project.getResource(ResourceType.TILESET).exists(tilesetId)) {
                throw new MapException("Tileset with ID '" + tilesetId + "' does not exist");
            }

            Tileset tileset = new Tileset(tilesetId);
            setTileset(tileset);
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Could not load the tileset: " + ex.getMessage());
        }
    }

    /**
     * Closes the current tileset.
     */
    protected void closeTileset() {

        if (!checkCurrentFileSaved()) {
            return;
        }

        setTileset(null);
    }

    public void save() {
        saveTileset();
    }

    /**
     * Saves the current tileset.
     */
    protected void saveTileset() {

        try {
            tileset.save();
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Could not save the tileset: " + ex.getMessage());
        }
    }
}
