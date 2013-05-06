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

import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;

/**
 * Options indicating how to display the map in the map view of the map editor.
 * This options affect only the map editor, not the game.
 * This class allows to set:
 * - what layers are displayed
 * - show or not the obstacle or non obstacle entities
 * - show the transparency
 */
public class MapViewRenderingOptions {

    /**
     * The map view affected by these options.
     */
    private MapView mapView;

    /**
     * Zoom of the map view.
     * The possible values are 0.25, 0.5, 1, 2 (default).
     */
    private double zoom;

    /**
     * Tells whether or not each layer is shown.
     */
    private boolean[] showLayers;

    /**
     * True to render the transparency, false to replace the transparent pixels
     * by a background color.
     * The transparency seems to make the program much slower with my Linux.
     */
    private boolean showTransparency;

    /**
     * Tells whether or not the grid is shown.
     */
    private boolean showGrid;

    /**
     * Size of a square of the grid (16 pixels by default).
     */
    private int gridSize;

    /**
     * Constructor.
     */
    public MapViewRenderingOptions(MapView mapView) {
        this.mapView = mapView;
        this.zoom = 2.0;
        this.showLayers = new boolean[] {true, true, true};
        this.showTransparency = true;
        this.showGrid = false;
        this.gridSize = 16;
    }

    /**
     * Returns the map.
     * @return the map
     */
    public Map getMap() {
        return mapView.getMap();
    }

    /**
     * Returns the zoom of the map view.
     * @return the zoom
     */
    public double getZoom() {
        return zoom;
    }

    /**
     * Sets the zoom of the map view.
     * @param zoom the zoom
     */
    public void setZoom(double zoom) {
        this.zoom = zoom;
        mapView.update(getMap(), null);
    }

    /**
     * Returns whether or not each layer is shown.
     * @return for each layer, true if it is shown and false otherwise
     */
    public boolean[] getShowLayers() {
        return showLayers;
    }

    /**
     * Returns whether or not a layer is shown.
     * @param layer a layer
     * @return true if the entities of this layer are shown, false otherwise
     */
    public boolean getShowLayer(Layer layer) {
        return showLayers[layer.getId()];
    }

    /**
     * Sets the layers shown.
     * @param showLowLayer true to show the entities of the low layer
     * @param showIntermediateLayer true to show the entities of the low layer
     * @param showHighLayer true to show the entities of the high layer
     */
    public void setShowLayers(boolean showLowLayer, boolean showIntermediateLayer, boolean showHighLayer) {
        showLayers[Layer.LOW.getId()] = showLowLayer;
        showLayers[Layer.INTERMEDIATE.getId()] = showIntermediateLayer;
        showLayers[Layer.HIGH.getId()] = showHighLayer;
        mapView.repaint();
    }

    /**
     * Sets whether or not a layer is shown.
     * @param layer a layer
     * @param show true to make the layer visible, false otherwise
     */
    public void setShowLayer(Layer layer, boolean show) {
        showLayers[layer.getId()] = show;
        mapView.repaint();
    }

    /**
     * Returns true if the transparency is rendered, i.e. if transparent pixels are
     * not drawn.
     * @return true if the transparency is shown, false if the transparent pixels
     * are replaced by a background color.
     */
    public boolean getShowTransparency() {
        return showTransparency;
    }

    /**
     * Sets whether or not the transparency is rendred.
     * @param showTransparency true to show the transparency (i.e. to make the transparent
     * pixels not drawn), false to replace the transparent pixels by a background color.
     *
     */
    public void setShowTransparency(boolean showTransparency) {
        this.showTransparency = showTransparency;
        mapView.repaint();
    }

    /**
     * Returns whether or not the grid is rendered.
     * @return true if the grid is shown.
     */
    public boolean getShowGrid() {
        return showGrid;
    }

    /**
     * Sets whether or not the gird is rendered.
     * @param showGrid true to show the grid.
     */
    public void setShowGrid(boolean showGrid) {
        this.showGrid = showGrid;
        mapView.repaint();
    }
    /**
     * Returns the current grid size in the map view
     * @return The size of a square of the grid in pixels.
     */
    public int getGridSize() {
        return gridSize;
    }

    /**
     * Sets the grid size in the map view.
     * @param gridSize The size of a square of the grid in pixels.
     */
    public void setGridSize(int gridSize) {
        this.gridSize = gridSize;
        mapView.repaint();
    }

    /**
     * Returns whether or not an entity is shown, depending on its type,
     * its layer and its obstacle property.
     * @param entity the entity considered
     * @return true if this entity is shown with the current options
     */
    public boolean isEntityShown(MapEntity entity) {
        Layer layer = entity.getLayer();
        return showLayers[layer.getId()];
    }
}

