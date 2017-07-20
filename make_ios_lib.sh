#!/bin/sh

lipo -create \
    lib_ios/x86_64/libcommon.a \
    lib_ios/arm64/libcommon.a \
    -o lib_ios/libcommon.a
lipo -create \
    lib_ios/x86_64/libexport.a \
    lib_ios/arm64/libexport.a \
    -o lib_ios/libexport.a
lipo -create \
    lib_ios/x86_64/libnetworkutils.a \
    lib_ios/arm64/libnetworkutils.a \
    -o lib_ios/libnetworkutils.a
lipo -create \
    lib_ios/x86_64/libstuncore.a \
    lib_ios/arm64/libstuncore.a \
    -o lib_ios/libstuncore.a
