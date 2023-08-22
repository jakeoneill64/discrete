#!/bin/bash

buildGlm(){

  cd glm
  cmake .
  make
  cd ..
  mkdir  ../include/glm
  mv glm/glm/libglm_static.a ../lib
  mv glm/glm/*.hpp ../include/glm
  mv glm/glm/gtc ../include/glm
  mv glm/glm/detail ../include/glm
  mv glm/glm/ext ../include/glm
  mv glm/glm/gtx ../include/glm
  mv glm/glm/simd ../include/glm

}

buildGlad(){

  cd glad
  cmake .
  make
  cd ..

  mv glad/libglad.a ../lib
  mv glad/include/glad/ ../include/
  mv glad/include/KHR ../include/

}

buildGlfw(){
  cd glfw
  cmake .
  make
  cd ..
  mv glfw/src/libglfw3.a ../lib
  mv glfw/include/GLFW/ ../include
}

buildBoost(){

  cd boost/
  git submodule init
  git submodule update
  ./bootstrap.sh
  ./b2

  [[ "$OSTYPE" == "darwin"* ]] && mv stage/lib/*.dylib ../../lib || mv stage/lib/*.a ../../lib

  mv boost ../../include

  cd ..

}

buildSpdlog(){
  cd spdlog
  cmake .
  make
  cd ..

  mv spdlog/libspdlog.a ../lib/
  mv spdlog/include/spdlog ../include

}

#TODO we need to make the module versions constant.

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

git submodule update --remote --init

# clean install
rm -rf ../lib/* ../include/*


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
buildJson
buildSpdlog
buildBoost










