// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2012 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.content.browser;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;

import org.chromium.ui.WindowAndroid;

/**
 * A version of {@link ContentView} that supports Honeycomb MR1 features.
 */
class HoneycombMR1ContentView extends ContentView {
    HoneycombMR1ContentView(Context context, int nativeWebContents, WindowAndroid windowAndroid,
            AttributeSet attrs, int defStyle) {
        super(context, nativeWebContents, windowAndroid, attrs, defStyle);
    }

    // Override implementation of ContentViewCore.InternalAccessDelegate in ContentView.
    @Override
    public boolean super_onGenericMotionEvent(MotionEvent event) {
        return super.onGenericMotionEvent(event);
    }

}
