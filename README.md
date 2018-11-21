#  HOT RELOADER for OF (and JUCE)

### INSTALLATION
#### **WINDOWS**: 
only change Configuration type to Dynamic Library (.dll) in project setting
Check C/C++ -> Code Generation -> Runtime Library need be same for plugin and app both
#### **OSX**: 
add new target Library to project (see https://forum.openframeworks.cc/t/can-i-change-empty-example-xcode-project-output-to-dylib/25456/3)
Build Settings -> Code Signing Identity set to Don't Code Sign (without this will be exception when loading dylib)
Build Phases -> add cpp sources & manually link with openFrameworks.a
* *for openFrameworks:*
1. Build Phases -> Run Script :
```bash
install_name_tool -change @executable_path/libfmodex.dylib @executable_path/../Frameworks/libfmodex.dylib "$TARGET_BUILD_DIR/lib$PRODUCT_NAME.dylib";
```
* *for JUCE: *
1. Build Phases -> Run Script :
```bash
install_name_tool -change @executable_path/libfmodex.dylib "$USER_LIBRARY_DIR/Application Support/com.company.app/libfmodex.dylib" "$TARGET_BUILD_DIR/lib$PRODUCT_NAME.dylib";
```
2. Copy libfmodex.dylib from of_release/libs/fmodex/lib/osx/libfmodex.dylib to ~/Library/Application Support/com.company.app/

### Alternatives
Similar project: https://github.com/mazbox/livecode-cplusplus/blob/master/Makefile

### TODO
* add async loading (thread):  bool async = false, async ? std::thread(&ofxHotreloader::load, this).detach() : load(); -> add recreation plugins in new callback and call at next check
* remove static loader -> create ofxPluginUtils object (maybe macros, see ofxPlugin)
