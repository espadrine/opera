// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.content.browser;

import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.os.Build;
import android.util.Log;
import android.view.ActionMode;
import android.view.KeyEvent;

import org.chromium.content.browser.SelectActionModeCallback.ActionHandler;

import java.net.URISyntaxException;

/**
 *  Main callback class used by ContentView.
 *
 *  This contains the superset of callbacks required to implement the browser UI and the callbacks
 *  required to implement the WebView API.
 *  The memory and reference ownership of this class is unusual - see the .cc file and ContentView
 *  for more details.
 *
 *  TODO(mkosiba): Rid this guy of default implementations. This class is used by both WebView and
 *  the browser and we don't want a the browser-specific default implementation to accidentally leak
 *  over to WebView.
 */
public class ContentViewClient {
    // Tag used for logging.
    private static final String TAG = "ContentViewClient";

    private CompatLayer mCompatLayer;

    public ContentViewClient() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            mCompatLayer = new HoneyCombCompatLayer();
        } else {
            mCompatLayer = new GenericCompatLayer();
        }
    }

    public void onUpdateTitle(String title) {
    }

    /**
     * Called whenever the background color of the page changes as notified by WebKit.
     * @param color The new ARGB color of the page background.
     */
    public void onBackgroundColorChanged(int color) {
    }

    /**
      * Lets client listen on the scaling changes on delayed, throttled
      * and best-effort basis. Used for WebView.onScaleChanged.
      */
    public void onScaleChanged(float oldScale, float newScale) {
    }

    /**
     * Notifies the client that the position of the top controls has changed.
     * @param topControlsOffsetYPix The Y offset of the top controls in physical pixels.
     * @param contentOffsetYPix The Y offset of the content in physical pixels.
     * @param overdrawBottomHeightPix The overdraw height.
     */
    public void onOffsetsForFullscreenChanged(
            float topControlsOffsetYPix, float contentOffsetYPix, float overdrawBottomHeightPix) {
    }

    public void onTabCrash() {
    }

    public void hidePopupDialog() {
    }

    public boolean shouldOverrideKeyEvent(KeyEvent event) {
        int keyCode = event.getKeyCode();
        // We need to send almost every key to WebKit. However:
        // 1. We don't want to block the device on the renderer for
        // some keys like menu, home, call.
        // 2. There are no WebKit equivalents for some of these keys
        // (see app/keyboard_codes_win.h)
        // Note that these are not the same set as KeyEvent.isSystemKey:
        // for instance, AKEYCODE_MEDIA_* will be dispatched to webkit.
        if (keyCode == KeyEvent.KEYCODE_MENU ||
            keyCode == KeyEvent.KEYCODE_HOME ||
            keyCode == KeyEvent.KEYCODE_BACK ||
            keyCode == KeyEvent.KEYCODE_CALL ||
            keyCode == KeyEvent.KEYCODE_ENDCALL ||
            keyCode == KeyEvent.KEYCODE_POWER ||
            keyCode == KeyEvent.KEYCODE_HEADSETHOOK ||
            keyCode == KeyEvent.KEYCODE_CAMERA ||
            keyCode == KeyEvent.KEYCODE_FOCUS ||
            keyCode == KeyEvent.KEYCODE_VOLUME_DOWN ||
            keyCode == KeyEvent.KEYCODE_VOLUME_MUTE ||
            keyCode == KeyEvent.KEYCODE_VOLUME_UP) {
            return true;
        }

        // We also have to intercept some shortcuts before we send them to the ContentView.
        if (mCompatLayer.isCtrlPressed(event) && (
                keyCode == KeyEvent.KEYCODE_TAB ||
                keyCode == KeyEvent.KEYCODE_W ||
                keyCode == KeyEvent.KEYCODE_F4)) {
            return true;
        }

        return false;
    }

    // Called when an ImeEvent is sent to the page. Can be used to know when some text is entered
    // in a page.
    public void onImeEvent() {
    }

    /**
     * Notified when a change to the IME was requested.
     *
     * @param requestShow Whether the IME was requested to be shown (may already be showing
     *                    though).
     */
    public void onImeStateChangeRequested(boolean requestShow) {
    }

    // TODO (dtrainor): Should expose getScrollX/Y from ContentView or make
    // computeHorizontalScrollOffset()/computeVerticalScrollOffset() public.
    /**
     * Gives the UI the chance to override each scroll event.
     * @param dx The amount scrolled in the X direction (in physical pixels).
     * @param dy The amount scrolled in the Y direction (in physical pixels).
     * @param scrollX The current X scroll offset (in physical pixels).
     * @param scrollY The current Y scroll offset (in physical pixels).
     * @return Whether or not the UI consumed and handled this event.
     */
    public boolean shouldOverrideScroll(float dx, float dy, float scrollX, float scrollY) {
        return false;
    }

    /**
     * Returns an ActionMode.Callback for in-page selection.
     */
    public Object getSelectActionModeCallback(
            Context context, ActionHandler actionHandler, boolean incognito) {
        return mCompatLayer.getSelectActionModeCallback(context, actionHandler, incognito);
    }

    /**
     * Called when the contextual ActionBar is shown.
     */
    public void onContextualActionBarShown() {
    }

    /**
     * Called when the contextual ActionBar is hidden.
     */
    public void onContextualActionBarHidden() {
    }

    /**
     * Called when a new content intent is requested to be started.
     */
    public void onStartContentIntent(Context context, String intentUrl) {
        Intent intent;
        // Perform generic parsing of the URI to turn it into an Intent.
        try {
            intent = Intent.parseUri(intentUrl, Intent.URI_INTENT_SCHEME);
        } catch (URISyntaxException ex) {
            Log.w(TAG, "Bad URI " + intentUrl + ": " + ex.getMessage());
            return;
        }

        try {
            context.startActivity(intent);
        } catch (ActivityNotFoundException ex) {
            Log.w(TAG, "No application can handle " + intentUrl);
        }
    }

    public void onExternalVideoSurfaceRequested(int playerId) {
    }

    /**
     * Called when the selection has changed.
     * @param anchorRect Selection anchor point in screen coordinates.
     * @param focusRect Selection focus point in screen coordinates.
     */
    public void onSelectionChanged(Rect anchorRect, Rect focusRect) {
    }

    /**
     * Called when a new selection preview should be shown.
     * @param previewBitmap Bitmap of preview content.
     * @param focusRect Selection focus point in screen coordinates.
     */
    public void onShowSelectionPreview(Bitmap previewBitmap, Rect focusRect) {
    }

    /**
     * Called when the current selection preview should hide.
     */
    public void onHideSelectionPreview() {
    }

    public void onGeometryChanged(int playerId, float x, float y, float width, float height) {
    }

    private interface CompatLayer {
        public boolean isCtrlPressed(KeyEvent event);
        public Object getSelectActionModeCallback(Context context, ActionHandler actionHandler, boolean incognito);
    }

    private class GenericCompatLayer implements CompatLayer {
        public boolean isCtrlPressed(KeyEvent event) {
            return false;
        }
        public Object getSelectActionModeCallback(Context context, ActionHandler actionHandler, boolean incognito) {
            return null;
        }
    }

    private class HoneyCombCompatLayer implements CompatLayer {
        public boolean isCtrlPressed(KeyEvent event) {
            return event.isCtrlPressed();
        }
        public Object getSelectActionModeCallback(Context context, ActionHandler actionHandler, boolean incognito) {
            return new SelectActionModeCallback(context, actionHandler, incognito);
        }
    }

    public ContentVideoViewClient getContentVideoViewClient() {
        return null;
    }
}
