Playback functionality is based on QtAV (wrapper around FFmpeg). UI is QML-based

Steps for building a project:
* Install latest Qt. Qt Creator is used for development
* Download and build latest QtAV SDK from GitHub (you can use version 1.13.0 from Tags for example)
https://github.com/wang-bin/QtAV
Install QtAV SDK using sdk_install.bat, this will integrate the QtAV library files into your Qt installation
* Open VideoWallpaper.pro in Qt Creator and build

Deployment:
* Project supports deployment to a "bin" folder. To enable it, add "make install" step to build settings in Qt Creator.