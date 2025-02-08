#!/bin/bash

buildGlm(){
  (
    cd glm
    cmake .
    make
    mkdir  ../../include/glm
    mv glm/libglm_static.a ../../lib
    mv glm/*.hpp ../../include/glm
    mv glm/gtc ../../include/glm
    mv glm/detail ../../include/glm
    mv glm/ext ../../include/glm
    mv glm/gtx ../../include/glm
    mv glm/simd ../../include/glm
    git clean -df
    git reset HEAD --hard
  )
}

buildGlfw(){
  (
    cd glfw
    cmake .
    make
    mv src/libglfw3.a ../../lib
    mv include/GLFW/ ../../include
    git clean -df
    git reset HEAD --hard
  )

}

buildGoogleTest(){
  (
    cd googletest
    git submodule update --init

    cmake .
    make

    for file in lib/*
    do
      cp "$file" ../../lib/
    done


    cp -r googletest/include/gtest ../../include/
    cp -r googlemock/include/gmock ../../include/
    git clean -df
    git reset HEAD --hard

  )
}

buildSpdlog(){
  (
    cd spdlog
    cmake .
    make

    mv libspdlog.a ../../lib/
    mv include/spdlog ../../include
    git clean -df
    git reset HEAD --hard
  )
}

cd modules

for d in *
do
  [[ -d "${d}" ]] || continue
  rm -rf "$d"
done

git submodule update --init

rm -rf ../lib
mkdir ../lib

buildGlfw
buildGlm
buildSpdlog
buildGoogleTest