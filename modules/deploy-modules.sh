#!/bin/bash

echo ${BASH_SOURCE[0]}

if [[ $(pwd) != $(dirname "${BASH_SOURCE[0]}") ]]; then
  echo you must be in the modules directory to run this script.
  exit 1
fi

sed=sed

git submodule foreach git reset --hard

if [[ $(uname) == "Darwin" ]]; then
   sed=gsed #lets not bother with bsd sed
fi

for d in *
do
  [[ -d "${d}" ]] || continue
  cd $d
  git clean -fd
  cd ..
done

$sed -i"" "81,82d" jsoncpp/CMakeLists.txt #remove the lines which prevent us from building in place.

for d in *
do
  [[ -d "${d}" ]] || continue
  cd $d
  cmake .
  make
  cd ..
done

rm -fr ../lib/* ../include/*

mkdir  ../include/gml

mv glad/libglad.a ../lib
mv glad/include/glad/ ../include/
mv glad/include/KHR ../include/

mv glfw/src/libglfw3.a ../lib
mv glfw/include/GLFW/ ../include

mv glm/glm/libglm_static.a ../lib
mv glm/glm/*.hpp ../include/gml

mv jsoncpp/lib/libjsoncpp.a ../lib
mv jsoncpp/include/json/ ../include
