
#  HOT RELOADER for openFrameworks / JUCE

### INSTALLATION
#### **WINDOWS**: 
only change Configuration type to Dynamic Library (.dll) in project setting
Check C/C++ -> Code Generation -> Runtime Library need be same for plugin and app both
#### **OSX**: 
##### Create project: ##### 
* *fast method*
generate project with generator, then show contents of generated *.xcodeproj, find *.pbxproj :
   1. change productType string to
```com.apple.product-type.library.dynamic```
   2. change name of project (above productType) to
```libNAME```
   3. change PRODUCT_NAME for debug to
   ```$(TARGET_NAME)```
   4. change shellScript string to script text below (only for JUCE)

* *long methog*
   add new target Library to project (see https://forum.openframeworks.cc/t/can-i-change-empty-example-xcode-project-output-to-dylib/25456/3)
   Build Settings -> Code Signing Identity set to Don't Code Sign (without this will be exception when loading dylib)
   Build Phases -> add cpp sources & manually link with openFrameworks.a

##### Modify project run script  (only for JUCE): ##### 
* *for openFrameworks:*
   Build Phases -> Run Script :
```bash
install_name_tool -change @executable_path/libfmodex.dylib @executable_path/../Frameworks/libfmodex.dylib "$TARGET_BUILD_DIR/lib$PRODUCT_NAME.dylib";
```
* *for JUCE:*
   Build Phases -> Run Script :
```bash
install_name_tool -change @executable_path/libfmodex.dylib "$USER_LIBRARY_DIR/Application Support/com.company.app/libfmodex.dylib" "$TARGET_BUILD_DIR/lib$PRODUCT_NAME.dylib";
```
##### Prepare app to run: ##### 
* *for openFrameworks:*
Copy libfmodex.dylib from of_release/libs/fmodex/lib/osx/libfmodex.dylib to app folder
* *for JUCE:*
Copy libfmodex.dylib from of_release/libs/fmodex/lib/osx/libfmodex.dylib to ~/Library/Application Support/com.company.app/

### Alternatives
Similar project: https://github.com/mazbox/livecode-cplusplus/blob/master/Makefile

### TODO
* add async loading (thread):  bool async = false, async ? std::thread(&ofxHotreloader::load, this).detach() : load(); -> add recreation plugins in new callback and call at next check
* remove static loader -> create ofxPluginUtils object (maybe macros, see ofxPlugin)
