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

import java.awt.*;

/**
 * Represents an animation of a sprite.
 */
public class SpriteAnimation {

    /**
     * @brief The directions of this animation.
     */
    private SpriteAnimationDirection[] directions;

    /**
     * @brief Interval in milliseconds between two frames
     * (this delay is the same for all directions),
     * 0 to let the sprite displayed (only possible when there is one frame)
     */
    private int frameDelay;

    /**
     * @brief Index of a frame to loop on when the animation is finished,
     * or -1 to make no loop.
     */
    private int loopOnFrame;

    /**
     * Creates an animation.
     * @param directions the list of directions of this animation
     * @param frameDelay interval in milliseconds between two frames
     * @param loopOnFrame index of a frame to loop on when the animation is finished, or -1
     */
    public SpriteAnimation(SpriteAnimationDirection[] directions, int frameDelay, int loopOnFrame) {
        this.directions = directions;
        this.frameDelay = frameDelay;
        this.loopOnFrame = loopOnFrame;
    }

    /**
     * Returns the origin point of this animation.
     * @param direction direction of animation
     * @return the corresponding origin point
     */
    public Point getOrigin(int direction) {

        return directions[direction].getOrigin();
    }

    /**
     * Returns the size of frames in this animation.
     * @param direction direction of animation
     * @return the corresponding size
     */
    public Dimension getSize(int direction) {

        return directions[direction].getSize();
    }

    /**
     * Returns a frame of this animation.
     * @param direction direction to use
     * @param frame index of the frame to show
     * @return the frame
     */
    public Image getFrame(int direction, int frame) {

        return directions[direction].getFrame(frame);
    }

    /**
     * Returns the number of directions in this animation.
     * @return The number of directions.
     */
    public int getNbDirections() {
        return directions.length;
    }

    /**
     * Returns a direction in this animation.
     * @param direction Index of the direction to get (the first one is 0).
     * @return The corresponding direction.
     */
    public SpriteAnimationDirection getDirection(int direction) {
        return directions[direction];
    }

    /**
     * @brief Returns the time interval between two frames.
     *
     * This delay is the same for all directions.
     *
     * @return The interval in milliseconds between two frames.
     * 0 means infinite (only possible when there is one frame).
     */
    public int getFrameDelay() {
        return frameDelay;
    }

    /**
     * @brief Returns the frame where the animation loops.
     * @returns The index of a frame where to the sprite loops after the last
     * frame, or -1 if there is no loop.
     */
    public int getLoopOnFrame() {
        return loopOnFrame;
    }

    /**
     * Displays a frame of this sprite.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     * @param direction direction of animation
     * @param frame index of the frame to get
     * @return the frame
     */
    public void paint(Graphics g, double zoom, boolean showTransparency,
            int x, int y, int direction, int frame) {

        directions[direction].paint(g, zoom, showTransparency, x, y, frame);
    }
}
