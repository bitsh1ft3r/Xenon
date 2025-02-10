# SPDX-FileCopyrightText: 2024 shadPS4 Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later

#!/bin/bash

if [[ -z $GITHUB_WORKSPACE ]]; then
	GITHUB_WORKSPACE="${PWD%/*}"
fi

# Prepare Tools for building the AppImage
wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget -q https://github.com/linuxdeploy/linuxdeploy-plugin-checkrt/releases/download/continuous/linuxdeploy-plugin-checkrt-x86_64.sh

chmod a+x linuxdeploy-x86_64.AppImage
chmod a+x linuxdeploy-plugin-checkrt-x86_64.sh

# Build AppImage
./linuxdeploy-x86_64.AppImage --appdir AppDir
./linuxdeploy-plugin-checkrt-x86_64.sh --appdir AppDir
./linuxdeploy-x86_64.AppImage --appdir AppDir -d "$GITHUB_WORKSPACE"/Xenon.desktop  -e "$GITHUB_WORKSPACE"/build/Xenon -i "$GITHUB_WORKSPACE"/Xenon/images/Xenon.png --output appimage
mv Xenon-x86_64.AppImage Xenon.AppImage
