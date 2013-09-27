// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.net;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Proxy;
import android.os.Build;

import org.chromium.base.CalledByNative;
import org.chromium.base.JNINamespace;
import org.chromium.base.NativeClassQualifiedName;

// This class partners with native ProxyConfigServiceAndroid to listen for
// proxy change notifications from Android.
@JNINamespace("net")
public class ProxyChangeListener {
    private static final String TAG = "ProxyChangeListener";
    private static boolean sEnabled = true;

    private int mNativePtr;
    private Context mContext;
    private ProxyReceiver mProxyReceiver;
    private Delegate mDelegate;
    private static ConnectivityManager sConnectivityManager;

    public interface Delegate {
        public void proxySettingsChanged();
    }

    private ProxyChangeListener(Context context) {
        mContext = context;
        sConnectivityManager = (ConnectivityManager)
            mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
    }

    private static boolean isMobileNetworkActive() {
        NetworkInfo info = sConnectivityManager.getActiveNetworkInfo();
        return (info != null &&
                info.isConnected() &&
                info.getType() == ConnectivityManager.TYPE_MOBILE);
    }

    public static void setEnabled(boolean enabled) {
        sEnabled = enabled;
    }

    public void setDelegateForTesting(Delegate delegate) {
        mDelegate = delegate;
    }

    @CalledByNative
    static public ProxyChangeListener create(Context context) {
        return new ProxyChangeListener(context);
    }

    static String sanitizeProxyAddress(String proxy) {
	// If the proxy is a IPv4 address (for example 127.0.0.1)
	// sanitize it by removing any leading zeros of every number
	// component.
	if (proxy != null && proxy.matches("^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$"))
	    return proxy.replaceAll("0*((?:[1-9][0-9]*)|0)", "$1");
	else
	    return proxy;
    }

    @SuppressWarnings("deprecation")
    @CalledByNative
    static public String getProperty(String property) {
        // pre-HONEYCOMB device and proxy property and mobile network
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.HONEYCOMB &&
            (property.endsWith(".proxyHost") || property.endsWith(".proxyPort")) &&
            isMobileNetworkActive()) {
            if (property.endsWith(".proxyHost")) {
                return sanitizeProxyAddress(Proxy.getDefaultHost());
            } else { // proxyPort
                return String.valueOf(Proxy.getDefaultPort());
            }
        }
        return System.getProperty(property);
    }

    @CalledByNative
    public void start(int nativePtr) {
        assert mNativePtr == 0;
        mNativePtr = nativePtr;
        registerReceiver();
    }

    @CalledByNative
    public void stop() {
        mNativePtr = 0;
        unregisterReceiver();
    }

    private class ProxyReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(Proxy.PROXY_CHANGE_ACTION) ||
                Build.VERSION.SDK_INT < Build.VERSION_CODES.HONEYCOMB &&
                intent.getAction().equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
                proxySettingsChanged();
            }
        }
    }

    private void proxySettingsChanged() {
        if (!sEnabled) {
            return;
        }
        if (mDelegate != null) {
          mDelegate.proxySettingsChanged();
        }
        if (mNativePtr == 0) {
          return;
        }
        // Note that this code currently runs on a MESSAGE_LOOP_UI thread, but
        // the C++ code must run the callbacks on the network thread.
        nativeProxySettingsChanged(mNativePtr);
    }

    private void registerReceiver() {
        if (mProxyReceiver != null) {
            return;
        }
        IntentFilter filter = new IntentFilter();
        filter.addAction(Proxy.PROXY_CHANGE_ACTION);
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.HONEYCOMB) {
            filter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
        }
        mProxyReceiver = new ProxyReceiver();
        mContext.getApplicationContext().registerReceiver(mProxyReceiver, filter);
    }

    private void unregisterReceiver() {
        if (mProxyReceiver == null) {
            return;
        }
        mContext.unregisterReceiver(mProxyReceiver);
        mProxyReceiver = null;
    }

    /**
     * See net/proxy/proxy_config_service_android.cc
     */
    @NativeClassQualifiedName("ProxyConfigServiceAndroid::JNIDelegate")
    private native void nativeProxySettingsChanged(int nativePtr);
}
