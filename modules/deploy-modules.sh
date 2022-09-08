#!/bin/bash

#TODO we need to make the module versions constant.

CURRENT_DIR="$(cd "$(dirname -- "$1")" >/dev/null; pwd -P)"

if [[ $(pwd) != $CURRENT_DIR ]]; then
  echo you must be in the modules directory to run this script.
  exit 1
fi

git clean -xfdf
git submodule foreach git reset --hard
git submodule foreach git clean -xfdf
git submodule foreach init
git submodule foreach update

sed=sed

if [[ $(uname) == "Darwin" ]]; then
   sed=gsed #lets not bother with bsd sed
fi

$sed -i"" "81,82d" jsoncpp/CMakeLists.txt #remove the lines which prevent us from building in place.

for d in *
do
  [[ -d "${d}" ]] || continue
  [[ "${d}" == "boost" ]] && continue
  cd $d
  cmake .
  make
  cd ..
done

rm -fr ../lib/* ../include/*

echo building boost
cd boost/
git clean -xfdf
git submodule foreach git reset --hard
git submodule foreach git clean -xfdf
git submodule init
git submodule update
./bootstrap.sh
./b2 --with-json --with-log --with-test
cd ..

echo deploying libraries
mkdir  ../include/glm

mv glad/libglad.a ../lib
mv glad/include/glad/ ../include/
mv glad/include/KHR ../include/

mv glfw/src/libglfw3.a ../lib
mv glfw/include/GLFW/ ../include

mv glm/glm/libglm_static.a ../lib
mv glm/glm/*.hpp ../include/glm
mv glm/glm/gtc ../include/glm
mv glm/glm/detail ../include/glm
mv glm/glm/ext ../include/glm
mv glm/glm/gtx ../include/glm
mv glm/glm/simd ../include/glm

mv jsoncpp/lib/libjsoncpp.a ../lib
mv jsoncpp/include/json/ ../include

if [[ "$OSTYPE" == "darwin"* ]]; then
  mv boost/stage/lib/libboost_unit_test_framework.dylib ../lib
else
  mv boost/stage/lib/libboost_unit_test_framework.so ../lib
fi
mv boost/boost/test ../include

mv boost/stage/lib/libboost_log.a ../lib
mv boost/boost/log ../include


