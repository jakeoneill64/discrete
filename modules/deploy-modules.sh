#!/bin/bash

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


# TODO add branch verification

mkdir ../include/glad ../include/glfw ../include/gml ../include/json

mv glad/libglad.a ../lib
mv glad/include/glad/glad.h ../include/glad/

mv glfw/src/libglfw3.a ../lib
mv glfw/include/GLFW/glfw3.h ../include/glfw

mv glm/glm/libglm_static.a ../lib
mv glm/glm/*.hpp ../include/gml

mv jsoncpp/lib/libjsoncpp.a ../lib
mv jsoncpp/include/json/*.h ../include/json
