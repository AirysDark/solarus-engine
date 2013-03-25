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

import java.awt.*;
import java.util.*;
import org.solarus.editor.*;
import org.solarus.editor.Map;

/**
 * Represents a tile for the map editor, i.e. a tile placed on the map.
 * A Tile is composed by a TilePattern (as created in the tileset editor)
 * and how the tile is placed on the map: its position, its layer and
 * how the pattern is repeated or not.
 */
public class Tile extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = null;

    /**
     * The tileset from which this tile is extracted.
     */
    protected Tileset tileset;

    /**
     * Creates a new tile on the map.
     * @param map the map
     * @throws MapException if the tile is not valid
     */
    public Tile(Map map) throws MapException {
        super(map, 0, 0);
        this.tileset = map.getTileset();
        this.layer = null;
    }

    /**
     * Creates a new tile from a dynamic tile.
     * @param dynamicTile A dynamic tile to copy.
     * @throws MapException if the tile is not valid.
     */
    public Tile(DynamicTile dynamicTile) throws MapException {
        this(dynamicTile.getMap());
        setIntegerProperty("pattern", dynamicTile.getTilePatternId());
        setLayer(dynamicTile.getLayer());
        setPositionInMap(dynamicTile.getPositionInMap());
        updateImageDescription();
    }

    /**
     * Sets a property specific to a tile.
     * @param name name of the property
     * @param value value of the property
     */
    public void setProperty(String name, String value) throws MapException {

        super.setProperty(name, value);

        if (name.equals("pattern")) {
            if (tileset == null) {
                return;
            }

            try {
                TilePattern tilePattern = getTilePattern(); // get the tile pattern from the tileset

                if (getWidth() == 0) {
                    setSize(tilePattern.getWidth(), tilePattern.getHeight());
                }

                if (layer == null) {
                    setLayer(tilePattern.getDefaultLayer());
                }
            }
            catch (NoSuchElementException ex) {
                throw new NoSuchTilePatternException(ex.getMessage());
            }
        }
    }

    /**
     * Changes the tileset used to represent this tile on the map.
     * The corresponding tile pattern from the new tileset (i.e. the tile pattern
     * with the same id) must have exactly the same properties,
     * otherwise a NoSuchTilePatternException is thrown.
     * @param tileset the tileset
     * @throws NoSuchTilePatternException if the new tileset could not be applied
     * because the tile pattern doesn't exist in this tileset or is different.
     */
    public void setTileset(Tileset tileset) throws NoSuchTilePatternException {

        if (tileset != this.tileset) {

            int tilePatternId = getTilePatternId();
            try {
                TilePattern newTilePattern = getTilePattern();

                // if a tileset was already defined, check that the
                // tile has the same properties
                if (this.tileset != null) {

                    TilePattern oldTilePattern = this.tileset.getTilePattern(tilePatternId);

                    if (!newTilePattern.equals(oldTilePattern)) {
                        throw new NoSuchTilePatternException("The tile pattern #" + tilePatternId + " is different in this tileset.");
                    }

                    // update the size on the map if the size in the tileset has changed
                    positionInMap.width = newTilePattern.getWidth() * getRepeatX();
                    positionInMap.height = newTilePattern.getHeight() * getRepeatY();
                }

                this.tileset = tileset;
            }
            catch (NoSuchElementException e) {
                throw new NoSuchTilePatternException("Unable to apply the tileset because the tile pattern #" + tilePatternId + " doesn't exist in this tileset.");
            }
        }
    }

    /**
     * Returns the id of the tile pattern in the tileset.
     * @return the id of the tile pattern in the tileset.
     */
    public int getTilePatternId() {
        return getIntegerProperty("pattern");
    }

    /**
     * Returns the tile pattern in the tileset.
     * @return the tile pattern in the tileset.
     */
    public TilePattern getTilePattern() {
        return tileset.getTilePattern(getTilePatternId());
    }

    /**
     * Returns whether the entity sets a specific layer when it is created
     * on a map.
     * @return true
     */
    public boolean hasInitialLayer() {
        return true;
    }

    /**
     * Returns the tile's obstacle property.
     * @return the tile's obstacle property
     */
    public Obstacle getObstacle() {
        return getTilePattern().getObstacle();
    }

    /**
     * Returns whether this kind of entity is allowed to have a name.
     * @return false.
     */
    public boolean canHaveName() {
        // Tiles are not allowed to have a name because they are optimized away
        // at runtime during the game.
        return false;
    }

    /**
     * Returns whether or not the entity is resizable.
     * A tile is resizable (i.e. its pattern can be replicated
     * horizontally or vertically).
     * @return true
     */
    public boolean isResizable() {
        return true;
    }

    /**
     * Returns the minimum size of the entity (for a resizable entity).
     * When the entity is resized, its new size must be a multiple of this minimum size.
     * @return the minimum size of the entity
     */
    public Dimension getUnitarySize() {

        if (tileset == null) { // special case when the tileset is not initialized yet
            return super.getUnitarySize();
        }

        TilePattern tilePattern = getTilePattern();
        return tilePattern.getSize();
    }

    /**
     * Returns the number of times the pattern is repeated on x.
     * @return the number of times the pattern is repeated on x
     */
    public int getRepeatX() {
        return positionInMap.width / getUnitarySize().width;
    }

    /**
     * Returns the number of times the pattern is repeated on y.
     * @return the number of times the pattern is repeated on y
     */
    public int getRepeatY() {
        return positionInMap.height / getUnitarySize().height;
    }

    /**
     * Draws the tile.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

        TilePattern tilePattern = getTilePattern();

        int x = getX();
        int y = getY();
        int repeatX = getRepeatX();
        int repeatY = getRepeatY();
        int width = tilePattern.getWidth();
        int height = tilePattern.getHeight();

        for (int i = 0; i < repeatY; i++) {
            for (int j = 0; j < repeatX; j++) {
                tilePattern.paint(g, tileset, x, y, zoom, showTransparency);
                x += width;
            }
            y += height;
            x = getX();
        }
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
        setIntegerProperty("pattern", null);
    }

    /**
     * Creates a dynamic tile with the same properties than this static tile.
     * @return the dynamic tile corresponding to this static tile
     * @throws QuestEditorException if the dynamic tile could not be created
     */
    public final DynamicTile createDynamicTile() throws QuestEditorException {
        return new DynamicTile(this);
    }
}

