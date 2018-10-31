# SUPER HOT RELOAD FOR OF

-  **WINDOWS**: only change Configuration type to Dynamic Library (.dll) in project setting
-  **OSX**: add new target Library to project (see https://forum.openframeworks.cc/t/can-i-change-empty-example-xcode-project-output-to-dylib/25456/3)
Build Settings -> Code Signing Identity set to Don't Code Sign
Build Phases -> add cpp sources & manually link with openFrameworks.a
copy libfmodex.dylib from of_release/libs/fmodex/lib/osx/libfmodex.dylib to app bin


Similar project: https://github.com/mazbox/livecode-cplusplus/blob/master/Makefile

**TODO**: 
add async loading (thread):
add async: bool async = false, async ? std::thread(&ofxHotreloader::load, this).detach() : load(); -> add recreation plugins in new callback and call at next check
