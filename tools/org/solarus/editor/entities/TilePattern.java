/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.entities;

import java.awt.*;
import java.awt.image.*;
import javax.swing.*;
import java.util.*;
import org.solarus.editor.*;

/**
 * Represents a tile pattern.
 * The pattern's dimensions must be multiples of 8 but this is not checked.
 */
public class TilePattern extends Observable {

    /**
     * Enumeration to identify the animation of a tile pattern.
     */
    public enum Animation {
        NONE,
        SEQUENCE_012,
        SEQUENCE_0121,
        SELF_SCROLLING,
        PARALLAX_SCROLLING,
        SEQUENCE_012_PARALLAX,
        SEQUENCE_0121_PARALLAX;

        public static final String[] humanNames = {
            "None",
            "Frames 1-2-3-1",
            "Frames 1-2-3-2-1",
            "Scrolling on itself",
            "Parallax scrolling",
            "Frames 1-2-3-1, parallax",
            "Frames 1-2-3-2-1, parallax",
        };

        public static Animation get(int id) {
            return values()[id];
        }

        public int getId() {
            return ordinal();
        }
    }

    /**
     * Enumeration to identify the animation separation of a tile pattern.
     */
    public enum AnimationSeparation {

        HORIZONTAL("animation_separation_horizontal.png"),
        VERTICAL("animation_separation_vertical.png");

        private String iconFileName;
        private static ImageIcon[] icons;

        public static final String[] humanNames = {
            "Horizontal",
            "Vertical",
        };

        private AnimationSeparation(String iconFileName) {
            this.iconFileName = iconFileName;
        }

        public static AnimationSeparation get(int id) {
            return values()[id];
        }

        public int getId() {
            return ordinal();
        }

        public static ImageIcon[] getIcons() {

            if (icons == null) {
                icons = new ImageIcon[values().length];
                int i = 0;
                for (AnimationSeparation value: values()) {
                    icons[i] = Project.getEditorImageIcon(value.iconFileName);
                    icons[i].setDescription(value.name());
                    i++;
                }
            }

            return icons;
        }
    }

    /**
     * Coordinates and dimensions of the tile pattern.
     *
     * For a multi-frame tile pattern, it is the size of the
     * big rectangle containing the 3 frames.
     */
    private Rectangle positionInTileset;

    /**
     * Subimages representing this tile pattern in the tileset for each zoom mode.
     */
    private BufferedImage[] images;

    /**
     * Obstacle property.
     */
    private Obstacle obstacle;

    /**
     * Type of animation.
     */
    private Animation animation;

    /**
     * Type of separation of the 3 animation frames in the tileset for an animated tile.
     */
    private AnimationSeparation animationSeparation;

    /**
     * Default layer of the tiles having this pattern.
     * It can be changed in the map editor once the tile is placed on a map.
     * Thus, several tiles with the same pattern can be on different layers of the map.
     */
    private Layer defaultLayer;

    /**
     * Simple constructor with no animation.
     * @param positionInTileset position of the tile pattern in the tileset
     * @param defaultLayer default layer of the tile pattern
     * @param obstacle the obstacle property of this tile pattern
     * @throws TilesetException if the tile size is incorrect
     */
    public TilePattern(Rectangle positionInTileset, Layer defaultLayer, Obstacle obstacle) throws TilesetException {
        this(positionInTileset, defaultLayer, obstacle, Animation.NONE, AnimationSeparation.HORIZONTAL);
    }

    /**
     * Constructor.
     * @param positionInTileset position of the tile pattern in the tileset
     * @param defaultLayer default layer of the tiles created with this pattern
     * @param obstacle the obstacle property of this tile pattern
     * @param animation type of animation
     * @param animationSeparation separation of the 3 animation frames in the tileset image
     * @throws TilesetException if the pattern size is incorrect
     */
    public TilePattern(Rectangle positionInTileset, Layer defaultLayer, Obstacle obstacle,
                Animation animation, AnimationSeparation animationSeparation) throws TilesetException {
        super();

        // check the width and the height
        if (positionInTileset.width <= 0 || positionInTileset.height <= 0 ||
            positionInTileset.width % 8 != 0 || positionInTileset.height % 8 != 0) {
            throw new TilesetException("The size of a tile must be positive and multiple of 8 pixels");
        }

        this.positionInTileset = positionInTileset;
        this.defaultLayer = defaultLayer;
        setAnimation(animation);
        setAnimationSeparation(animationSeparation);
        setObstacle(obstacle);

        this.images = new BufferedImage[4];
    }

    /**
     * Returns the rectangle of the tile pattern.
     * If the tile pattern is animated, a rectangle containing the 3 frames
     * is returned.
     * @return the pattern's rectangle
     */
    public Rectangle getPositionInTileset() {
        return positionInTileset;
    }

    /**
     * Returns the x coordinate of the tile pattern in the tileset image.
     * @return the x coordinate of the tile pattern in the tileset image
     */
    public int getX() {
        return positionInTileset.x;
    }

    /**
     * Returns the y coordinate of the tile pattern in the tileset image.
     * @return the y coordinate of the tile pattern in the tileset image
     */
    public int getY() {
        return positionInTileset.y;
    }

    /**
     * Returns the tile pattern's width.
     * If the tile pattern is animated, the value returned is the width
     * of one frame.
     * @return the tile pattern's width in pixels
     */
    public int getWidth() {

        int width = positionInTileset.width;

        if (isMultiFrame() && animationSeparation == AnimationSeparation.HORIZONTAL) {
            width = width / 3;
        }

        return width;
    }

    /**
     * Returns the tile pattern's height.
     * If the tile pattern is animated, the value returned is the height
     * of one frame.
     * @return the tile pattern's height in pixels
     */
    public int getHeight() {

        int height = positionInTileset.height;

        if (isMultiFrame() && animationSeparation == AnimationSeparation.VERTICAL) {
            height = height / 3;
        }

        return height;
    }

    /**
     * Returns the tile pattern's size.
     * @return the tile pattern's size in pixels
     */
    public Dimension getSize() {
        return new Dimension(getWidth(), getHeight());
    }

    /**
     * Returns the tile pattern's obstacle property.
     * @return the obstacle property
     */
    public Obstacle getObstacle() {
        return obstacle;
    }

    /**
     * Changes the tile pattern's obstacle property.
     * @param obstacle the obstacle propery
     * @throws TilesetException if the obstacle specified is diagonal
     * and the tile pattern is not square
     */
    public void setObstacle(Obstacle obstacle) throws TilesetException {

        // diagonal obstacle: check that the tile is square
        if (obstacle.isDiagonal() && getWidth() != getHeight()) {
            throw new TilesetException("Cannot make a diagonal obstacle on a non-square tile pattern (size is " + getWidth() + "x" + getHeight() + ")");
        }

        this.obstacle = obstacle;
        setChanged();
        notifyObservers();
    }

    /**
     * Returns the default layer of the tiles having this pattern.
     * @return the default layer of the tile pattern
     */
    public Layer getDefaultLayer() {
        return defaultLayer;
    }

    /**
     * Changes the default layer the tiles having this pattern.
     * @param defaultLayer the default layer of the tile pattern
     */
    public void setDefaultLayer(Layer defaultLayer) {
        if (defaultLayer != this.defaultLayer) {
            this.defaultLayer = defaultLayer;
            setChanged();
            notifyObservers();
        }
    }

    /**
     * Returns the tile pattern's animation type.
     * @return the tile pattern's animation type
     */
    public Animation getAnimation() {
        return animation;
    }

    /**
     * Sets the tile pattern's animation type.
     * If the specified animation makes the tile animated, a valid separation is chosen
     * between AnimationSeparation.HORIZONTAL and AnimationSeparation.VERTICAL.
     * You can change it with setAnimationSeparation().
     * If both directions are invalid, an TilesetException is thrown.
     * @param animation the tile's animation type
     * @throws TilesetException if the tile is animated but cannot be divided in 3 frames
     */
    public void setAnimation(Animation animation) throws TilesetException {

        if (isMultiFrame(animation)) {

            // try to set the animation separation
            int width = positionInTileset.width;
            int height = positionInTileset.height;

            // try to divide the tile pattern in the biggest direction
            if (width >= height) {
                try {
                    trySetAnimationSeparation(AnimationSeparation.HORIZONTAL);
                }
                catch (TilesetException e) {
                    trySetAnimationSeparation(AnimationSeparation.VERTICAL);
                    // an exception is thrown if this doesn't work either
                }
            }
            else {
                try {
                    trySetAnimationSeparation(AnimationSeparation.VERTICAL);
                }
                catch (TilesetException e) {
                    trySetAnimationSeparation(AnimationSeparation.HORIZONTAL);
                }
            }
        }

        this.animation = animation;

        setChanged();
        notifyObservers();
    }

    /**
     * Returns whether or not the tile pattern has an animation with several frames.
     * @return true if the tile pattern is multi-frame, false otherwise
     */
    public boolean isMultiFrame() {
        return isMultiFrame(animation);
    }

    /**
     * Returns whether the specified animation type corresponds to a multi-frame animation.
     * @param animation an animation type
     * @return true if this animation is multi-frame
     */
    public boolean isMultiFrame(Animation animation) {
        return animation == Animation.SEQUENCE_012 || animation == Animation.SEQUENCE_0121
          || animation == Animation.SEQUENCE_012_PARALLAX || animation == Animation.SEQUENCE_0121_PARALLAX;
    }

    /**
     * Returns the type of separation of the 3 animation frames if the tile pattern is animated.
     * @return the type of separation of the 3 animation frames
     */
    public AnimationSeparation getAnimationSeparation() {
        return animationSeparation;
    }

    /**
     * Sets the type of separation of the 3 animation frames if the tile pattern is animated.
     * Nothing is done if the tile pattern is not animated.
     * @param animationSeparation the type of separation of the 3 animation frames
     * @throws TilesetException thrown if the separation is not valid, i.e.
     * if the size of the 3 frames formed are not multiple of 8.
     */
    public void setAnimationSeparation(AnimationSeparation animationSeparation) throws TilesetException {

        if (!isMultiFrame()) {
            return;
        }

        trySetAnimationSeparation(animationSeparation);

        setChanged();
        notifyObservers();
    }

    /**
     * Sets the type of separation of the 3 animation frames.
     * @param animationSeparation the type of separation of the 3 animation frames
     * @throws TilesetException thrown if the separation is not valid, i.e.
     * if the size of the 3 frames formed are not multiple of 8.
     */
    private void trySetAnimationSeparation(AnimationSeparation animationSeparation) throws TilesetException {

        // check that the tile pattern can be separated in 3 frames
        if (animationSeparation == AnimationSeparation.HORIZONTAL
            && positionInTileset.width % 24 != 0) {
            throw new TilesetException("Cannot divide the tile in 3 frames : the size of each frame must be a multiple of 8 pixels");
        }
        else if (animationSeparation == AnimationSeparation.VERTICAL
                 && positionInTileset.height % 24 != 0) {
            throw new TilesetException("Cannot divide the tile in 3 frames : the size of each frame must be a multiple of 8 pixels");
        }

        this.animationSeparation = animationSeparation;
    }

    /**
     * Compares this tile pattern to another one.
     * @return true if it the tile patterns have exactly the same properties
     */
    public boolean equals(Object other) {

        if (!(other instanceof TilePattern)) {
            return false;
        }

        TilePattern tilePattern = (TilePattern) other;

        return obstacle == tilePattern.obstacle
            && animation == tilePattern.animation
            && animationSeparation == tilePattern.animationSeparation
            && positionInTileset.equals(tilePattern.positionInTileset);
    }

    /**
     * Returns the image representing this tile pattern in its tileset with a zoom mode.
     * @param zoom the zoom
     * @param tileset the tileset
     */
    public BufferedImage getTileImage(Tileset tileset, double zoom) {

        int index;
        if (zoom == 0.25) {
            index = 0;
        }
        else if (zoom == 0.5) {
            index = 1;
        }
        else if (zoom == 1.0) {
            index = 2;
        }
        else {
            index = 3;
        }

        if (images[index] == null) {
            int x = (int) Math.round(getX() * zoom);
            int y = (int) Math.round(getY() * zoom);
            int width = (int) Math.round(getWidth() * zoom);
            int height = (int) Math.round(getHeight() * zoom);
            images[index] = tileset.getScaledImage(index).getSubimage(x, y, width, height);
        }

        return images[index];
    }

    /**
     * Draws the tile pattern on a component.
     * @param g graphic context
     * @param tileset the tileset
     * @param x x coordinate of where the tile pattern has to be painted
     * @param y y coordinate of where the tile pattern has to be painted
     * @param zoom scale of the image (1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, Tileset tileset, int x, int y, double zoom, boolean showTransparency) {

        BufferedImage patternImage = getTileImage(tileset, zoom);
        int dx = (int) Math.round(x * zoom);
        int dy = (int) Math.round(y * zoom);

/*
        Color bgColor = showTransparency ? null : MapEntity.bgColor;
        g.drawImage(getTileImage(tileset, zoom), dx, dy, bgColor, this);
*/

        if (!showTransparency) {
            g.setColor(MapEntity.bgColor);
            g.fillRect(dx, dy, patternImage.getWidth(), patternImage.getHeight());
        }
        g.drawImage(patternImage, dx, dy, null);
    }
}
