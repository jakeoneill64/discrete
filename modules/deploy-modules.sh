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
  ./b2 --with-json --with-test
  cd ..

  if [[ "$OSTYPE" == "darwin"* ]]; then
    mv boost/stage/lib/libboost_unit_test_framework.dylib ../lib
  else
    mv boost/stage/lib/libboost_unit_test_framework.so ../lib
  fi
  mv boost/libs/test/include/boost/ ../include

}

buildJson(){

  $sed -i"" "81,82d" jsoncpp/CMakeLists.txt #remove the lines which prevent us from building in place.

  cd jsoncpp
  cmake .
  make
  cd ..

  mv jsoncpp/lib/libjsoncpp.a ../lib
  mv jsoncpp/include/json/ ../include
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

CURRENT_DIR="$(cd "$(dirname -- "$1")" >/dev/null; pwd -P)"

if [[ $(pwd) != $CURRENT_DIR ]]; then
  echo you must be in the modules directory to run this script.
  exit 1
fi

for d in *
do
  [[ -d "${d}" ]] || continue
  rm -rf $d
done

git submodule init
git submodule update

sed=sed

if [[ $(uname) == "Darwin" ]]; then
   sed=gsed #lets not bother with bsd sed
fi

#clean install
rm -fr ../lib/* ../include/*





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

buildBoost &
buildGlad &
buildGlfw &
buildGlm &
buildJson &
buildSpdlog








