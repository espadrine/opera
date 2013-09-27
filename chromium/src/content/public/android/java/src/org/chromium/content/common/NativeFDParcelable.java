// Copyright (c) 2012-2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.content.common;

import java.io.IOException;
import java.io.FileDescriptor;

import android.os.Parcel;
import android.os.Parcelable;
import android.os.ParcelFileDescriptor;

import org.chromium.base.JNINamespace;

@JNINamespace("content")
class NativeFDParcelable implements Parcelable {
    private int mFd = -1;
    private boolean mClose = false;

    private NativeFDParcelable(int fd) {
        mFd = fd;
        mClose = true;
    }

    public synchronized int detachFd() {
        mClose = false;
        return mFd;
    }

    public static NativeFDParcelable fromFd(int fd) throws IOException {
        int newFd = nativeDup(fd);
        if (newFd < 0)
            throw new IOException("Invalid file descriptor");

        return new NativeFDParcelable(newFd);
    }

    public static NativeFDParcelable adoptFd(int fd) {
        return new NativeFDParcelable(fd);
    }

    public void finalize() {
        try {
            close();
        } catch (IOException e) {
        }
    }

    public synchronized void close() throws IOException {
        if (mClose) {
            nativeClose(mFd);
            mClose = false;
        }
    }

    private static native void nativeSetFD(FileDescriptor fileDescriptor, int fd);
    private static native int nativeGetFD(FileDescriptor fileDescriptor);
    private static native int nativeDup(int fd);
    private static native void nativeClose(int fd);

    // Implement Parcelable

    public int describeContents() {
        return 0;
    }

    public void writeToParcel(Parcel out, int flags) {
        FileDescriptor fileDescriptor = new FileDescriptor();
        nativeSetFD(fileDescriptor, mFd);
        out.writeFileDescriptor(fileDescriptor);
    }

    public static final Parcelable.Creator<NativeFDParcelable> CREATOR
        = new Parcelable.Creator<NativeFDParcelable>() {
            public NativeFDParcelable createFromParcel(Parcel in) {
                return new NativeFDParcelable(in);
            }

            public NativeFDParcelable[] newArray(int size) {
                return new NativeFDParcelable[size];
            }
        };

    private NativeFDParcelable(Parcel in) {
        mFd = nativeDup(nativeGetFD(in.readFileDescriptor().getFileDescriptor()));
    }
}
