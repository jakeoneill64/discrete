#!/bin/bash

buildGlm(){
  (
    cd glm || exit
    cmake .
    make
    mkdir  ../../include/glm
    cp glm/*.a ../../lib
    cp -r glm/*.hpp ../../include/glm
    cp -r glm/gtc ../../include/glm
    cp -r glm/detail ../../include/glm
    cp -r glm/ext ../../include/glm
    cp -r glm/gtx ../../include/glm
    cp -r glm/simd ../../include/glm
    git clean -df
    git reset HEAD --hard
  )
}

buildGlfw(){
  (
    cd glfw || exit
    cmake .
    make
    cp src/libglfw3.a ../../lib
    cp -r include/GLFW/ ../../include/GLFW
    git clean -df
    git reset HEAD --hard
  )

}

buildGoogleTest(){
  (
    cd googletest || exit
    git submodule update --init

    cmake .
    make

    for file in lib/*
    do
      [[ "$file" =~ .*main.a ]] && continue
      cp "$file" ../../lib/
    done


    cp -r googletest/include/gtest ../../include/gtest
    cp -r googlemock/include/gmock ../../include/gmock
    git clean -df
    git reset HEAD --hard

  )
}

buildSpdlog(){
  (
    cd spdlog || exit
    cmake .
    make

    cp libspdlog.a ../../lib/
    cp -r include/spdlog ../../include/spdlog
    git clean -df
    git reset HEAD --hard
  )
}

cd modules || exit

for d in *
do
  [[ -d "${d}" ]] || continue
  rm -rf "$d"
done

git submodule update --init

rm -rf ../lib
rm -rf ../include
mkdir ../lib
mkdir ../include

buildGlfw
buildGlm
buildSpdlog
buildGoogleTest