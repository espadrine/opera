// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.content.browser.input;

import android.view.View;

/**
 * CursorController for selecting a range of text.
 */
public abstract class SelectionHandleController implements CursorController, HandleView.Listener {

    // The following constants match the ones in
    // third_party/WebKit/public/web/WebTextDirection.h
    private static final int TEXT_DIRECTION_DEFAULT = 0;
    private static final int TEXT_DIRECTION_LTR = 1;
    private static final int TEXT_DIRECTION_RTL = 2;

    /** The cursor controller images, lazily created when shown. */
    private HandleView mStartHandle, mEndHandle;

    /** Whether handles should show automatically when text is selected. */
    private boolean mAllowAutomaticShowing = true;

    /** Whether selection anchors are active. */
    private boolean mIsShowing;

    private View mParent;

    private int mFixedHandleX;
    private int mFixedHandleY;

    private int mWindowPadding;

    public SelectionHandleController(View parent) {
        mParent = parent;
    }

    /** Automatically show selection anchors when text is selected. */
    public void allowAutomaticShowing() {
        mAllowAutomaticShowing = true;
    }

    /** Hide selection anchors, and don't automatically show them. */
    public void hideAndDisallowAutomaticShowing() {
        hide();
        mAllowAutomaticShowing = false;
    }

    @Override
    public boolean isShowing() {
        return mIsShowing;
    }

    @Override
    public void hide() {
        if (mIsShowing) {
            if (mStartHandle != null) mStartHandle.hide();
            if (mEndHandle != null) mEndHandle.hide();
            mIsShowing = false;
        }
    }

    void cancelFadeOutAnimation() {
        hide();
    }

    /**
     * Updates the selection for a movement of the given handle. The handle
     * can be either the start or end handle, but it must represent the focus
     * point. Note that this will not actually result in the handle moving to
     * (x,y): moveSelectionFocus(x,y) will modify the selection and set the
     * actual coordinates later via set[Start|End]HandlePosition.
     */
    @Override
    public void updatePosition(HandleView handle, int x, int y) {
        if (x != mFixedHandleX || y != mFixedHandleY) {
            moveSelectionFocus(x, y);
        }
    }

    @Override
    public void beforeStartUpdatingPosition(HandleView handle) {
        HandleView fixedHandle = (handle == mStartHandle) ? mEndHandle : mStartHandle;
        mFixedHandleX = fixedHandle.getAdjustedPositionX();
        mFixedHandleY = fixedHandle.getLineAdjustedPositionY();

        if (handle.isAnchor()) {
            swapSelectionAnchorAndFocus();
        }
    }

    /**
     * Swaps the selection anchor and focus points. Conceptually the anchor
     * point is fixed while the focus point can move. Swapping the anchor and
     * focus points enables us to move both visual points while internally
     * we're only moving the focus point.
     */
    protected abstract void swapSelectionAnchorAndFocus();

    /**
     * Moves the selection focus point.
     * @param x New X-position.
     * @param y New Y-position.
     */
    protected abstract void moveSelectionFocus(int x, int y);

    /**
     * @return true iff this controller is being used to move the selection start.
     */
    boolean isSelectionStartDragged() {
        return mStartHandle != null && mStartHandle.isDragging();
    }

    /**
     * @return true iff this controller is being used to drag either the selection start or end.
     */
    public boolean isDragging() {
        return (mStartHandle != null && mStartHandle.isDragging()) ||
               (mEndHandle != null && mEndHandle.isDragging());
    }

    /**
     * @return Pixel width of selection handle.
     */
    public int getHandleWidth() {
        assert mStartHandle.getDrawable().getIntrinsicWidth() ==
                mEndHandle.getDrawable().getIntrinsicWidth();
        return mStartHandle.getDrawable().getIntrinsicWidth();
    }

    /**
     * @return Pixel height of selection handle.
     */
    public int getHandleHeight() {
        assert mStartHandle.getDrawable().getIntrinsicHeight() ==
                mEndHandle.getDrawable().getIntrinsicHeight();
        return mStartHandle.getDrawable().getIntrinsicHeight();
    }

    @Override
    public void onTouchModeChanged(boolean isInTouchMode) {
        if (!isInTouchMode) {
            hide();
        }
    }

    @Override
    public void onDetached() {}

    /**
     * Moves the start handle so that it points at the given coordinates.
     * @param x The start handle position X in physical pixels.
     * @param y The start handle position Y in physical pixels.
     */
    public void setStartHandlePosition(float x, float y) {
        mStartHandle.positionAt((int) x, (int) y);
    }

    /**
     * Moves the end handle so that it points at the given coordinates.
     * @param x The end handle position X in physical pixels.
     * @param y The end handle position Y in physical pixels.
     */
    public void setEndHandlePosition(float x, float y) {
        mEndHandle.positionAt((int) x, (int) y);
    }

    /**
     * If the handles are not visible, sets their visibility to View.VISIBLE and begins fading them
     * in.
     */
    public void beginHandleFadeIn() {
        mStartHandle.beginFadeIn();
        mEndHandle.beginFadeIn();
    }

    /**
     * Sets the start and end handles to the given visibility.
     */
    public void setHandleVisibility(int visibility) {
        mStartHandle.setVisibility(visibility);
        mEndHandle.setVisibility(visibility);
    }

    /**
     * Shows the handles if allowed.
     *
     * @param startDir Direction (left/right) of start handle.
     * @param endDir Direction (left/right) of end handle.
     */
    public void onSelectionChanged(int startDir, int endDir, boolean startIsAnchor) {
        if (mAllowAutomaticShowing) {
            showHandles(startDir, endDir, startIsAnchor);
        }
    }

    /**
     * Sets both start and end position and show the handles.
     * Note: this method does not trigger a selection, see
     * moveSelectionFocus()
     *
     * @param startDir Direction (left/right) of start handle.
     * @param endDir Direction (left/right) of end handle.
     * @param startIsAnchor true if the start point is the anchor point.
     */
    public void showHandles(int startDir, int endDir, boolean startIsAnchor) {
        if (mStartHandle != null)
            mStartHandle.setIsAnchor(startIsAnchor);
        if (mEndHandle != null)
            mEndHandle.setIsAnchor(!startIsAnchor);
        createHandlesIfNeeded(startDir, endDir);
        showHandlesIfNeeded();
    }

    /**
     * Set window offset in pixels.
     *
     * @param padding The pixel offset for the parent view.
     */
    public void setWindowPadding(int padding) {
        mWindowPadding = padding;
        if (mStartHandle != null)
            mStartHandle.setWindowPadding(mWindowPadding);
        if (mEndHandle != null)
            mEndHandle.setWindowPadding(mWindowPadding);
    }

    private void createHandlesIfNeeded(int startDir, int endDir) {
        if (mStartHandle == null) {
            mStartHandle = new HandleView(this,
                startDir == TEXT_DIRECTION_RTL ? HandleView.RIGHT : HandleView.LEFT, mParent);
            mStartHandle.setWindowPadding(mWindowPadding);
            mStartHandle.setListener(this);
        }
        if (mEndHandle == null) {
            mEndHandle = new HandleView(this,
                endDir == TEXT_DIRECTION_RTL ? HandleView.LEFT : HandleView.RIGHT, mParent);
            mEndHandle.setWindowPadding(mWindowPadding);
            mEndHandle.setListener(this);
        }
    }

    private void showHandlesIfNeeded() {
        if (!mIsShowing) {
            mIsShowing = true;
            mStartHandle.show();
            mEndHandle.show();
            setHandleVisibility(HandleView.VISIBLE);
        }
    }
}
