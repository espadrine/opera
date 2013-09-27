This README explains how to build a custom native library for Opera Desktop.

Due to platform limitations, we have different build instructions for different
desktop platforms.

Mac build instructions
----------------------

First extract the archive, put downloaded Opera (<Next/Developer>).app in
chromium/src/out/Release subdirectory of the extracted folder, rename
"Opera <Next/Developer>.app" to "Opera.app" so that the final product can find
it. Then download libopera.a built for Mac for the same version (extract it from
a zip archive if that's what you downloaded), and put it in the same directory
as Opera.app.

After that following commands can be used to build the open source part of Opera
browser. Since a large amount of this is based on Chromium open source project,
you can refer to their website (http://www.chromium.org/) for further
information about how to tweak certain build parameters.

  git clone https://git.chromium.org/chromium/tools/depot_tools.git
  export PATH="$PATH":`pwd`/depot_tools
  export GYP_GENERATORS=ninja
  cd opera-<version>/chromium/src
  tools/clang/scripts/update.sh
  build/gyp_chromium -I ../../desktop/opera_global.gypi \
      --depth=. ../../desktop/opera_source_release.gyp
  ninja -C out/Release opera_framework

Windows build instructions
--------------------------

To be able to run the final product of Windows build, you need to obtain a
dynamically linked Opera build (shared library version) provided along with this
source code package. From provided sources you may build dlls for the open
source parts, and replace the ones from Opera with them.

  cd chromium\src\build
  python gyp_chromium -Dcomponent=shared_library

Open all.sln in Visual Studio 2010. From this solution you are able to modify
and build dll libraries in "Release" configuration and then replace them with
the ones in Opera shared library installation.
