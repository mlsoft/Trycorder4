# CMake generated Testfile for 
# Source directory: /home/mlsoft/AndroidStudioProjects/Trycorder4/ktryclient
# Build directory: /home/mlsoft/AndroidStudioProjects/Trycorder4/ktryclient/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(appstreamtest "cmake" "-DAPPSTREAMCLI=/usr/bin/appstreamcli" "-DINSTALL_FILES=/home/mlsoft/AndroidStudioProjects/Trycorder4/ktryclient/build/install_manifest.txt" "-P" "/usr/share/ECM/kde-modules/appstreamtest.cmake")
