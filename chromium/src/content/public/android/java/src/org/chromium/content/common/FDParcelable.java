// Copyright (c) 2012-2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.content.common;

import java.io.IOException;

import android.os.Parcel;
import android.os.Parcelable;
import android.os.ParcelFileDescriptor;
import android.util.Log;

import org.chromium.content.common.NativeFDParcelable;

public class FDParcelable implements Parcelable {
    private static final String TAG = "FDParcelable";

    private static boolean sUseFallback = false;

    private FDWrapper mFdWrapper;

    static {
        if (android.os.Build.VERSION.SDK_INT <
                android.os.Build.VERSION_CODES.HONEYCOMB_MR2)
            sUseFallback = true;

        if (sUseFallback)
            Log.w(TAG, "Using fallback method for transferring file descriptors.");
    }

    private FDParcelable(FDWrapper fdWrapper) {
        mFdWrapper = fdWrapper;
    }

    public int detachFd() {
        return mFdWrapper.detachFd();
    }

    public static FDParcelable fromFd(int fd) throws IOException {
        if (sUseFallback)
            return new FDParcelable(FallbackFDWrapper.fromFd(fd));
        else
            return new FDParcelable(DefaultFDWrapper.fromFd(fd));
    }

    public static FDParcelable adoptFd(int fd) {
        if (sUseFallback)
            return new FDParcelable(FallbackFDWrapper.adoptFd(fd));
        else
            return new FDParcelable(DefaultFDWrapper.adoptFd(fd));
    }

    public void close() throws IOException {
        mFdWrapper.close();
    }

    // Implements Parcelable

    public int describeContents() {
        return 0;
    }

    public void writeToParcel(Parcel out, int flags) {
        mFdWrapper.writeToParcel(out, flags);
    }

    public static final Parcelable.Creator<FDParcelable> CREATOR
        = new Parcelable.Creator<FDParcelable>() {
            public FDParcelable createFromParcel(Parcel in) {
                if (sUseFallback)
                    return new FDParcelable(FallbackFDWrapper.createFromParcel(in));
                else
                    return new FDParcelable(DefaultFDWrapper.createFromParcel(in));
            }

            public FDParcelable[] newArray(int size) {
                return new FDParcelable[size];
            }
        };

    // Backward compatibility

    private interface FDWrapper {
        public int detachFd();
        public void close() throws IOException;
        public void writeToParcel(Parcel out, int flags);
    }

    private static class FallbackFDWrapper implements FDWrapper {
        private NativeFDParcelable mFd;

        private FallbackFDWrapper(NativeFDParcelable fd) {
            mFd = fd;
        }

        public int detachFd() {
            return mFd.detachFd();
        }

        public void close() throws IOException {
            mFd.close();
        }

        public void writeToParcel(Parcel out, int flags) {
            mFd.writeToParcel(out, flags);
        }

        public static FDWrapper fromFd(int fd) throws IOException {
            return new FallbackFDWrapper(NativeFDParcelable.fromFd(fd));
        }

        public static FDWrapper adoptFd(int fd) {
            return new FallbackFDWrapper(NativeFDParcelable.adoptFd(fd));
        }

        public static FDWrapper createFromParcel(Parcel in) {
            return new FallbackFDWrapper(NativeFDParcelable.CREATOR.createFromParcel(in));
        }
    }

    private static class DefaultFDWrapper implements FDWrapper {
        private ParcelFileDescriptor mFd;

        private DefaultFDWrapper(ParcelFileDescriptor fd) {
            mFd = fd;
        }

        public int detachFd() {
            return mFd.detachFd();
        }

        public void close() throws IOException {
            mFd.close();
        }

        public void writeToParcel(Parcel out, int flags) {
            mFd.writeToParcel(out, flags);
        }

        public static FDWrapper fromFd(int fd) throws IOException {
            return new DefaultFDWrapper(ParcelFileDescriptor.fromFd(fd));
        }

        public static FDWrapper adoptFd(int fd) {
            return new DefaultFDWrapper(ParcelFileDescriptor.adoptFd(fd));
        }

        public static FDWrapper createFromParcel(Parcel in) {
            return new DefaultFDWrapper(ParcelFileDescriptor.CREATOR.createFromParcel(in));
        }
    }
}
