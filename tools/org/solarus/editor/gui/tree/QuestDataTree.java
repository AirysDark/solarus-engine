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
package org.solarus.editor.gui.tree;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.Observable;
import java.util.Observer;
import javax.swing.JComponent;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.JTree;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import org.solarus.editor.Map;
import org.solarus.editor.Project;
import org.solarus.editor.Resource;
import org.solarus.editor.ResourceType;
import org.solarus.editor.QuestEditorException;
import org.solarus.editor.entities.Tileset;
import org.solarus.editor.gui.EditorWindow;
import org.solarus.editor.gui.FileEditorWindow;
import org.solarus.editor.gui.MapEditorWindow;
import org.solarus.editor.gui.TilesetEditorWindow;
import org.solarus.editor.gui.GuiTools;

/**
 * A tree that shows the whole resource database of the game:
 * maps, tilesets, sprites, enemies, etc.
 */
public class QuestDataTree extends JTree implements TreeSelectionListener, Observer {
    private String projectPath;
    private String quest;
    private EditorWindow editorWindow;
    private QuestDataTreeTilesetPopupMenu tilesetPopupMenu;
    private QuestDataTreeMapPopupMenu mapPopupMenu;
    private QuestDataTreePopupMenu popupMenu;
    public QuestDataTree(String quest, EditorWindow parent) {
        this.quest = quest;
        this.editorWindow = parent;
        addTreeSelectionListener(this);
        addMouseListener(new QuestDataTreeMouseAdapter());
        
        mapPopupMenu = new QuestDataTreeMapPopupMenu();
        popupMenu = new QuestDataTreePopupMenu();
        tilesetPopupMenu = new QuestDataTreeTilesetPopupMenu();
    }
    /**
     * Reload the tree, rebuilding the model from
     * the resources.
     */
    public void reloadTree() {
        setModel(new EditorTreeModel(projectPath));
        repaint();        
    }
    
    public void setRoot(String projectPath) {
        this.projectPath = projectPath;
        reloadTree();
    }

    public void valueChanged(TreeSelectionEvent e) {
        //System.out.println(e.getNewLeadSelectionPath());
    }

    public void addMap(Map map) {
        DefaultMutableTreeNode mapNode = (DefaultMutableTreeNode) treeModel.getChild(treeModel.getRoot(), ResourceType.MAP.ordinal());
        ResourceElement element = new ResourceElement(ResourceType.MAP, map.getId(), map.getName());
        ((EditorTreeModel) treeModel).insertNodeInto(new DefaultMutableTreeNode(element), mapNode, mapNode.getChildCount());
        map.addObserver(this);
        repaint();
    }

    public void addTileset(Tileset tileset) {
        DefaultMutableTreeNode tilesetNode = (DefaultMutableTreeNode) treeModel.getChild(treeModel.getRoot(), ResourceType.TILESET.ordinal());
        ResourceElement element = new ResourceElement(ResourceType.TILESET, tileset.getId(), tileset.getName());
        tilesetNode.add(new DefaultMutableTreeNode(element));
        repaint();
    }

    public void update(Observable o, Object arg) {
        repaint();
    }

    class EditorTreeModel extends DefaultTreeModel {

        public EditorTreeModel(String path) {
            super((new DefaultMutableTreeNode("Quest")));
            for (ResourceType resourceType : ResourceType.values()) {
                DefaultMutableTreeNode resourceNode = new DefaultMutableTreeNode(resourceType.getName());
                ((DefaultMutableTreeNode) getRoot()).add(resourceNode);
                if (Project.isLoaded()) {
                    addChildren(resourceNode, resourceType);
                }
            }
        }

        @Override
        public boolean isLeaf(Object e) {
            return ((DefaultMutableTreeNode) e).getChildCount() == 0 || !(((DefaultMutableTreeNode) e).getUserObject() instanceof String);
        }

        protected final void addChildren(DefaultMutableTreeNode parentNode, ResourceType resourceType) {
            Resource resource = Project.getResource(resourceType);
            String[] ids = resource.getIds();

            try {
                for (int i = 0; i < ids.length; i++) {
                    parentNode.add(new DefaultMutableTreeNode(new ResourceElement(resourceType, ids[i], resource.getElementName(ids[i]))));
                }
            }
            catch (QuestEditorException ex) {
                GuiTools.errorDialog("Unexpected error while building the quest tree: " + ex.getMessage());
            }
        }
    }

    class QuestDataTreeMouseAdapter extends MouseAdapter {

        @Override
        public void mousePressed(MouseEvent e) {
            DefaultMutableTreeNode clickedNode = null;
            try {
                if (e.getButton() == MouseEvent.BUTTON3) {
                    clickedNode = (DefaultMutableTreeNode) QuestDataTree.this.getSelectionPath().getLastPathComponent();
                    if (clickedNode.isLeaf()) {
                        // right click: show a popup menu if the element is a map
                        int row = QuestDataTree.this.getRowForLocation(e.getX(), e.getY());
                        if (row == -1) {
                            return;
                        }
                        QuestDataTree.this.setSelectionRow(row);
                        clickedNode = (DefaultMutableTreeNode) QuestDataTree.this.getSelectionPath().getLastPathComponent();
                        ResourceElement element = (ResourceElement) clickedNode.getUserObject();
                        if (element.type == ResourceType.MAP) {
                            popupMenu.setMap(element.id);
                            popupMenu.show((JComponent) e.getSource(),
                                    e.getX(), e.getY());
                        }
                    }
                    else {
                        //Show the popup menu when right-click on map
                        if (clickedNode.getUserObject().equals(ResourceType.MAP.getName())) {
                            mapPopupMenu.show((JComponent) e.getSource(),
                                    e.getX(), e.getY());
                        }
                        //Popup menu when right-click on tileset
                        else if (clickedNode.getUserObject().equals(ResourceType.TILESET.getName())) {
                            tilesetPopupMenu.show((JComponent) e.getSource(),
                                    e.getX(), e.getY());                                                      
                        }
                    }
                } else if (e.getClickCount() == 2) {
                    // double-click: open the clicked file

                    clickedNode = (DefaultMutableTreeNode) QuestDataTree.this.getSelectionPath().getLastPathComponent();
                    if (clickedNode.isLeaf()) {
                        ResourceElement element = (ResourceElement) clickedNode.getUserObject();

                        switch (element.type) {

                            case MAP:
                            {
                                Map m = new Map(element.id);
                                MapEditorWindow mapEditor = new MapEditorWindow(quest, editorWindow, m);
                                editorWindow.addEditor(mapEditor);
                                m.addObserver(editorWindow);
                                break;
                            }

                            case TILESET:
                            {
                                Tileset t = new Tileset(element.id);
                                TilesetEditorWindow tileEditor = new TilesetEditorWindow(quest, editorWindow, t);
                                editorWindow.addEditor(tileEditor);
                                break;
                            }

                            case LANGUAGE:
                            {
                                /* TODO uncomment this when the dialog editor works
                                Dialogs d = new Dialogs(element.id);
                                DialogsEditorWindow dialogsEditor = new DialogsEditorWindow(quest, editorWindow, d);
                                editorWindow.addEditor(dialogsEditor);
                                */
                                File f = Project.getDialogsFile(element.id);
                                FileEditorWindow fileEditor = new FileEditorWindow(quest, editorWindow, f);

                                editorWindow.addEditor(fileEditor);
                                break;
                            }

                            case ENEMY:
                            {
                                File f = new File(Project.getEnemyScriptFile(element.id));
                                FileEditorWindow fileEditor = new FileEditorWindow(quest, editorWindow, f);
                                editorWindow.addEditor(fileEditor);
                                break;
                            }

                            case ITEM:
                            {
                                File f = new File(Project.getItemScriptFile(element.id));
                                FileEditorWindow fileEditor = new FileEditorWindow(quest, editorWindow, f);
                                editorWindow.addEditor(fileEditor);
                                break;
                            }

                            case SPRITE:
                            {
                                File f = new File(Project.getSpriteFile(element.id).getAbsolutePath());
                                FileEditorWindow fileEditor = new FileEditorWindow(quest, editorWindow, f);
                                editorWindow.addEditor(fileEditor);
                                break;
                            }
                        }
                    }
                }
            }
            catch (QuestEditorException ex) {
                GuiTools.errorDialog(ex.getMessage());
            }
        }
    }
    /**
     * Popup menu associated to the right click on the map sub-tree
     */
    class QuestDataTreeMapPopupMenu extends JPopupMenu implements ActionListener {    
        private JMenuItem newMapMenu;
        public QuestDataTreeMapPopupMenu()
        {
            newMapMenu = new JMenuItem("New Map");
            newMapMenu.addActionListener(this);
            add(newMapMenu);            
        }
        @Override
        public void actionPerformed(ActionEvent e)
        {
            MapEditorWindow mapEditor = new MapEditorWindow(quest, editorWindow);
            mapEditor.newMap();
            editorWindow.addEditor(mapEditor);
            mapEditor.getMap().addObserver(editorWindow);
        }
        
    }
    /**
     * Popup menu associated to the right click on the tileset sub-tree
     */
    class QuestDataTreeTilesetPopupMenu extends JPopupMenu implements ActionListener {    
        private JMenuItem newTilesetMenu;
        public QuestDataTreeTilesetPopupMenu()
        {
            newTilesetMenu = new JMenuItem("New Tileset");
            newTilesetMenu.addActionListener(this);
            add(newTilesetMenu);            
        }
        @Override
        public void actionPerformed(ActionEvent e)
        {
            TilesetEditorWindow tilesetEditor = new TilesetEditorWindow(quest, editorWindow);
            tilesetEditor.newTileset();
            editorWindow.addEditor(tilesetEditor);            
        }
        
    }

    /**
     * Popup menu associated to maps in the tree.
     */
    class QuestDataTreePopupMenu extends JPopupMenu implements ActionListener {

        private String mapId;
        private JMenuItem mapMenu, scriptMenu, deleteMenu;

        public QuestDataTreePopupMenu() {
            mapMenu = new JMenuItem("Open Map");
            add(mapMenu);
            mapMenu.addActionListener(this);
            scriptMenu = new JMenuItem("Open Map Script");
            add(scriptMenu);
            scriptMenu.addActionListener(this);
            deleteMenu = new JMenuItem("Delete Map");
            add(deleteMenu);
            deleteMenu.addActionListener(this);
        }

        public void actionPerformed(ActionEvent e) {

            if (e.getSource() == mapMenu) {
                // open the map
                try {
                    Map map = new Map(mapId);
                    MapEditorWindow mapEditor = new MapEditorWindow(quest, editorWindow, map);
                    editorWindow.addEditor(mapEditor);
                    map.addObserver(editorWindow);
                }
                catch (QuestEditorException ex) {
                    GuiTools.errorDialog("Could not load the map: " + ex.getMessage());
                }
            }
            //delete the map
            else if (e.getSource() == deleteMenu) {
                try {
                    int answer = JOptionPane.showConfirmDialog(this,
                            "Are you sure you want to delete the map " + mapId + " ?",
                            "Are you sure ?",
                            JOptionPane.YES_NO_OPTION,
                            JOptionPane.WARNING_MESSAGE);
                    if (answer == JOptionPane.YES_OPTION) {
                        Map.delete(mapId);
                        Project.getResource(ResourceType.MAP).removeElement(mapId);
     
                        //TODO: Do it in a cleaner way
                        //Here we reload the whole tree, which is not 
                        //the most optimized way of removing the child
                        //from the tree.
                        reloadTree();
                        
                        repaint();
                    }
                }
                catch (QuestEditorException ex) {
                    GuiTools.errorDialog("Could not delete the map: " + ex.getMessage());
                }
            } else {
                // open the script
                File mapScritFile = Project.getMapScriptFile(mapId);
                FileEditorWindow fileEditor = new FileEditorWindow(quest, editorWindow, mapScritFile);
                editorWindow.addEditor(fileEditor);
            }
        }

        public void setMap(String mapId) {
            this.mapId = mapId;
        }
    }

    /**
     * Stores the id of an element from a resource, its name and its
     * resource type.
     */
    class ResourceElement {

        public final ResourceType type;
         public final String id;
        public final String name;

        public ResourceElement(ResourceType type, String id, String name) {
            this.type = type;
            this.id = id;
            this.name = name;
        }

        public String toString() {
            return name;
        }
    }
}
