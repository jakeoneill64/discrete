#!/bin/bash

buildGlm(){

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
  git reset head --hard
  cd ..


}

buildGlad(){

  cd glad
  cmake .
  make

  mv libglad.a ../../lib
  mv include/glad/ ../../include/
  mv include/KHR ../../include/
  git clean -df
  git reset head --hard
  cd ..

}

buildGlfw(){
  cd glfw
  cmake .
  make
  mv src/libglfw3.a ../../lib
  mv include/GLFW/ ../../include
  git clean -df
  git reset head --hard
  cd ..

}

buildBoost(){

  cd boost/
  git submodule update --init
  ./bootstrap.sh
  ./b2

  [[ "$OSTYPE" == "darwin"* ]] && mv stage/lib/*.dylib ../../lib || mv stage/lib/*.a ../../lib

  mv boost ../../include
  git clean -df
  git reset head --hard

  cd ..

}

buildSpdlog(){
  cd spdlog
  cmake .
  make

  mv libspdlog.a ../../lib/
  mv include/spdlog ../../include
  git clean -df
  git reset head --hard
  cd ..

}

CURRENT_DIR="$(cd "$(dirname  "$0")" >/dev/null; pwd -P)"

if [[ $(pwd) != "$CURRENT_DIR" ]]; then
  echo 'you must be in the modules directory to run this script.'
  exit 1
fi

for d in *
do
  [[ -d "${d}" ]] || continue
  rm -rf $d
done

git submodule update --init

# clean install
rm -rf ../lib ../include
mkdir ../lib ../include


#         (
#    (     )
#    )    (
#    (   _ )  ))
# ,-----' |  ((
# | //  : |   )
# | //  : | )/
# | //  : |
# `-----._|
#  _/___\_
# [_______]

buildGlad
buildGlfw
buildGlm
buildSpdlog
buildBoost










