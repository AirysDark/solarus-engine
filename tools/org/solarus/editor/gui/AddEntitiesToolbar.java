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
import java.awt.event.*;
import javax.swing.*;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;

/**
 * A toolbar to choose an entity to add onto the map.
 */
public class AddEntitiesToolbar extends JComponent {

    /**
     * Defines the entities available in the toolbar.
     * Each element is a type of entity and a possible subtype.
     */
    private static Cell[] cells = {
        new Cell(EntityType.DESTINATION),
        new Cell(EntityType.TELETRANSPORTER),
        new Cell(EntityType.PICKABLE),
        new Cell(EntityType.DESTRUCTIBLE, Destructible.Subtype.GRASS),
        new Cell(EntityType.DESTRUCTIBLE, Destructible.Subtype.BUSH),
        new Cell(EntityType.DESTRUCTIBLE, Destructible.Subtype.POT),
        new Cell(EntityType.DESTRUCTIBLE, Destructible.Subtype.STONE_SMALL_WHITE),
        new Cell(EntityType.CHEST),
        new Cell(EntityType.JUMPER),
        new Cell(EntityType.ENEMY),
        new Cell(EntityType.NPC, NPC.Subtype.GENERALIZED_NPC),
        new Cell(EntityType.NPC, NPC.Subtype.USUAL_NPC),
        new Cell(EntityType.BLOCK),
        new Cell(EntityType.SWITCH, Switch.Subtype.WALKABLE_VISIBLE),
        new Cell(EntityType.WALL),
        new Cell(EntityType.SENSOR),
        new Cell(EntityType.CRYSTAL),
        new Cell(EntityType.CRYSTAL_BLOCK, CrystalBlock.Subtype.ORANGE),
        new Cell(EntityType.CRYSTAL_BLOCK, CrystalBlock.Subtype.BLUE),
        new Cell(EntityType.SHOP_ITEM),
        new Cell(EntityType.CONVEYOR_BELT),
        new Cell(EntityType.DOOR),
        new Cell(EntityType.STAIRS, Stairs.Subtype.SPIRAL_UPSTAIRS),
        new Cell(EntityType.STAIRS, Stairs.Subtype.INSIDE_FLOOR),
    };

    /**
     * Represents an entity avaiable in the toolbar.
     */
    private static class Cell {

        private final EntityType type;
        private final EntitySubtype subtype;

        public Cell(EntityType type) {
            this(type, null);
        }

        public Cell(EntityType type, EntitySubtype subtype) {
            this.type = type;
            this.subtype = subtype;
        }

        public EntityType getType() {
            return type;
        }

        public EntitySubtype getSubtype() {
            return subtype;
        }
    }

    /**
     * The map view associated to the toolbar.
     */
    private MapView mapView;

    /**
     * Description of each image in the toolbar.
     */
    private EntityImageDescription[] imageDescriptions;

    /**
     * Creates a new toolbar to add entities.
     * @param mapView the map view where the entities will be added
     */
    public AddEntitiesToolbar(MapView mapView) {
        super();
        this.mapView = mapView;
        mapView.setAddEntitiesToolbar(this); // to be notified when the map view state changes
        addMouseListener(new AddEntitiesToolbarMouseListener());

        // count the number of cells to build
        int nbCells = cells.length;
        this.imageDescriptions = new EntityImageDescription[nbCells];

        for (int i = 0; i < nbCells; i++) {
            EntityImageDescription imageDescription = MapEntity.getImageDescription(cells[i].getType(), cells[i].getSubtype());
            this.imageDescriptions[i] = imageDescription;
        }
    }

    /**
     * Returns the preferred size of the component.
     * @return the preferred size of the component
     */
    public Dimension getPreferredSize() {
        return new Dimension(700, 33);
    }

    /**
     * Draws the component.
     * @param g the graphic context
     */
    public void paint(Graphics g) {
        super.paint(g);

        // draw a line to separate the toolbar from the component above
        g.setColor(Color.black);
        g.drawLine(0, 0, getWidth(), 0);

        if (!Project.isLoaded()) {
            // if no project is loaded, we cannot access the image files
            return;
        }

        // get the entity type being added (if any)
        EntityType entityTypeBeingAdded = mapView.getEntityTypeBeingAdded();
        EntitySubtype entitySubtypeBeingAdded = mapView.getEntitySubtypeBeingAdded();

        // draw the icons for all types of entities (except TileOnMap which has its own dedicated view)
        for (int i = 0; i < cells.length; i++) {

            // draw the image
            int x = i * 16;
            int y = 0;
            Rectangle positionInDestinationImage = new Rectangle(x, y, 16, 16);
            imageDescriptions[i].paint(g, 2, true, positionInDestinationImage);

            // draw the selection rectangle if we are currently adding this kind of entity
            if (entityTypeBeingAdded != null &&
                    entityTypeBeingAdded == cells[i].getType() &&
                    entitySubtypeBeingAdded == cells[i].getSubtype()) {
                int scaledX = x * 2;
                int scaledY = y * 2;
                g.setColor(Color.RED);
                g.drawRect(scaledX, scaledY, 32, 32);
                g.drawRect(scaledX + 1, scaledY + 1, 30, 30);
            }
        }
    }

    /**
     * The mouse listener associated to this component.
     */
    private class AddEntitiesToolbarMouseListener extends MouseAdapter {

        /**
         * Constructor.
         */
        public AddEntitiesToolbarMouseListener() {

        }

        /**
         * This method is called when the mouse is pressed onto the component.
         */
        public void mousePressed(MouseEvent ev) {

            int cell = ev.getX() / 32;

            if (mapView.getMap() == null || cell >= cells.length) {
                return;
            }

            EntityType typeClicked = cells[cell].getType();
            EntitySubtype subtypeClicked = cells[cell].getSubtype();
            mapView.startAddingEntity(typeClicked, subtypeClicked);
        }
    }
}
