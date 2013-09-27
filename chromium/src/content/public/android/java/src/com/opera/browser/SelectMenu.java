// Copyright (c) 2012 Opera Software.

package com.opera.browser;

import android.content.Context;
import android.graphics.Rect;
import android.graphics.Point;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;

import org.chromium.content.R;

/**
 * SelectMenu provides a bubble like popup menu with actions to be
 * performed on the current selection.
 */
public abstract class SelectMenu extends FrameLayout {
    private final PopupWindow mWindow;
    private final View mParent;

    // Rect to which the bubble should be anchored above or below.
    private Rect mAnchorRect;

    // When shown below mAnchorRect, to not overlap on the selection handlers,
    // the vertical space between the popup window and mAnchorRect
    private final int mVerticalSpaceShownBelow = 140;

    // Top left position of the popup window when show above mAnchorRect.
    private Point mWindowAbovePos = new Point();

    // Top left position of the popup window when show below mAnchorRect.
    private Point mWindowBelowPos = new Point();

    // Top left position of the popup window when show in the middle of mAnchorRect
    private Point mWindowMiddlePos = new Point();

    private TextView pasteButton;
    private View pasteSeparator;

    private ImageView mArrowUp;
    private ImageView mArrowDown;

    // whether arrow image is attached to the top of
    // R.id.chromium_select_menu_bubble_container, if false, attached to its bottom
    private boolean mAttachBubbleContainerTop = false;

    private Rect mBubbleContainerPadding = new Rect();
    private ViewGroup.MarginLayoutParams mBubbleContainerParams;

    // true if the select menu is active.
    private boolean mIsShowing;

    /**
     * Creates a new selection menu.
     *
     * @param context Associated context.
     * @param parent Parent view.
     */
    public SelectMenu(Context context, View parent) {
        super(context);

        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.chromium_select_menu, this, true);

        LinearLayout bubbleContainer = (LinearLayout) findViewById(R.id.chromium_select_menu_bubble_container);

        mArrowUp = (ImageView) findViewById(R.id.chromium_select_menu_arrow_up);
        mArrowDown= (ImageView) findViewById(R.id.chromium_select_menu_arrow_down);

        bubbleContainer.getBackground().getPadding(mBubbleContainerPadding);

        mBubbleContainerParams = (ViewGroup.MarginLayoutParams) bubbleContainer.getLayoutParams();

        TextView textView = (TextView) findViewById(R.id.chromium_select_menu_copy);
        textView.setText(context.getText(R.string.select_menu_copy));
        textView.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                hide();
                onCopy();
            }
        });

        pasteButton = (TextView) findViewById(R.id.chromium_select_menu_paste);
        pasteButton.setText(context.getText(R.string.select_menu_paste));
        pasteButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                hide();
                onPaste();
            }
        });

        pasteSeparator = findViewById(R.id.chromium_select_menu_separator2);

        textView = (TextView) findViewById(R.id.chromium_select_menu_search);
        textView.setText(context.getText(R.string.select_menu_search));
        textView.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                hide();
                onSearch();
            }
        });

        this.mParent = parent;
        this.mWindow = new PopupWindow(this, LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);

        // Window positioning depends on the view size. Since the view size is
        // not known until after the first layout, register a listener and
        // reposition the window after doing the initial layout.
        ViewTreeObserver viewTreeObserver = getViewTreeObserver();
        if (viewTreeObserver.isAlive()) {
            viewTreeObserver.addOnGlobalLayoutListener(new OnGlobalLayoutListener() {
                @Override
                public void onGlobalLayout() {
                    getViewTreeObserver().removeGlobalOnLayoutListener(this);

                    anchorAt(mAnchorRect);
                }
            });
        }
    }

    /**
     * @return true if the selection section is visible
     */
    private boolean isSelectionVisible(Rect parentRect) {
        // Rescale first selection rect to be in the same coordinates system
        final ViewParent parent = mParent.getParent();
        Rect rescaledAnchorRect = new Rect(mAnchorRect);
        parent.getChildVisibleRect(mParent, rescaledAnchorRect, null);
        return Rect.intersects(rescaledAnchorRect, parentRect);
    }

    /**
     * @return true if the selection menu should be visible in relation to its
     * parent view.
     */
    private boolean isPositionVisible(Point windowPos, Rect parentRect) {
        return windowPos.y >= parentRect.top &&
            windowPos.y + getMeasuredHeight() <= parentRect.bottom;
    }

    /**
     * @return visible rectangle of parent view.
     */
    private Rect getParentRect() {
        final ViewParent parent = mParent.getParent();
        if (parent == null) {
            return new Rect();
        }

        // Get the visible rectangle from our parent in relation to its parent.
        Rect clip = new Rect(0, 0, mParent.getWidth(), mParent.getHeight());
        parent.getChildVisibleRect(mParent, clip, null);

        return clip;
    }

    /**
     * Switch to show above or below mAnchorRect
     * @param above true to show above, false to show below
     */
    private void switchAboveBelow(boolean above) {
        if (above) {
            mArrowUp.setVisibility(View.GONE);
            mArrowDown.setVisibility(View.VISIBLE);
            mAttachBubbleContainerTop = false;

        } else {
            mArrowUp.setVisibility(View.VISIBLE);
            mArrowDown.setVisibility(View.GONE);
            mAttachBubbleContainerTop = true;
        }
    }

    /**
     * Anchors the selection menu to the specified position. The selection
     * menu will be floating above the anchor point.
     * @param rect The popup window will first try to show above rect, if there
     *             is not enough room, it will try to show below rect
     */
    public void anchorAt(Rect rect) {
        mAnchorRect = rect;
        Rect parentRect = getParentRect();
        if (!isSelectionVisible(parentRect)) {
            hide();
            return;
        }

        mWindowAbovePos.x = mWindowBelowPos.x = mWindowMiddlePos.x =
                mAnchorRect.left + mAnchorRect.width() / 2 - getMeasuredWidth() / 2;

        mWindowAbovePos.y = mAnchorRect.top - getMeasuredHeight();
        mWindowBelowPos.y = mAnchorRect.bottom + mVerticalSpaceShownBelow;
        mWindowMiddlePos.y = (parentRect.bottom - parentRect.top) / 2;

        if (mWindow.isShowing()) {
            switchAboveBelow(true);
            mWindow.update(mWindowAbovePos.x, mWindowAbovePos.y, -1, -1);

            if (!isPositionVisible(mWindowAbovePos, parentRect)) {
                hide();

                switchAboveBelow(false);
                if (isPositionVisible(mWindowBelowPos, parentRect)) {
                    mWindow.showAtLocation(mParent, 0, mWindowBelowPos.x, mWindowBelowPos.y);
                    mIsShowing = true;
                } else {
                    switchAboveBelow(true);
                    if (isPositionVisible(mWindowMiddlePos, parentRect)) {
	                    mWindow.showAtLocation(mParent, 0, mWindowMiddlePos.x, mWindowMiddlePos.y);
	                    mIsShowing = true;
                    }
                }
            }
        }
    }

    /**
     * @return true if the selection menu is active.
     */
    public boolean isShowing() {
        return mIsShowing;
    }

    /**
     * Shows the selection menu if it's currently anchored to a visible area.
     */
    public void show() {
        if (!mIsShowing) {
            Rect parentRect = getParentRect();
            if (!isSelectionVisible(parentRect)) return;
            switchAboveBelow(true);
            if (isPositionVisible(mWindowAbovePos, parentRect)) {
                mWindow.showAtLocation(mParent, 0, mWindowAbovePos.x, mWindowAbovePos.y);
                mIsShowing = true;
            } else {
                switchAboveBelow(false);
                if (isPositionVisible(mWindowBelowPos, parentRect)) {
                    mWindow.showAtLocation(mParent, 0, mWindowBelowPos.x, mWindowBelowPos.y);
                    mIsShowing = true;
                } else {
                    switchAboveBelow(true);
                    if (isPositionVisible(mWindowMiddlePos, parentRect)) {
	                    mWindow.showAtLocation(mParent, 0, mWindowMiddlePos.x, mWindowMiddlePos.y);
	                    mIsShowing = true;
                    }
                }
            }
        }
    }

    /**
     * Hides the selection menu.
     */
    public void hide() {
        if (mIsShowing) {
            mWindow.dismiss();
            mIsShowing = false;
        }
    }

    /**
     * Enables or disables the paste button.
     * @param enable true to enable, false to disable.
     */
    public void enablePaste(boolean enable) {
        if (enable) {
            pasteButton.setVisibility(View.VISIBLE);
            pasteSeparator.setVisibility(View.VISIBLE);
        } else {
            pasteButton.setVisibility(View.GONE);
            pasteSeparator.setVisibility(View.GONE);
        }
    }

    /**
     * Called when copy button is pressed.
     */
    public abstract void onCopy();

    /**
     * Called when paste button is pressed.
     */
    public abstract void onPaste();

    /**
     * Called when search button is pressed.
     */
    public abstract void onSearch();

    public void updateMarginAndPadding(boolean attachBubbleContainerTop) {
        if (attachBubbleContainerTop) {
            mBubbleContainerParams.topMargin = -mBubbleContainerPadding.top;
            mBubbleContainerParams.bottomMargin = 0;
        } else {
            mBubbleContainerParams.topMargin = 0;
            mBubbleContainerParams.bottomMargin = -mBubbleContainerPadding.bottom;
        }
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        updateMarginAndPadding(mAttachBubbleContainerTop);
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
    }
}
