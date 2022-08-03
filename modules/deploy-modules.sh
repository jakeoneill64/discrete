#!/bin/bash

sed=sed

if [[ $(uname) == "Darwin" ]]; then
   sed=gsed #let not bother with bsd sed
fi


for d in *
do
  cd $d
  cmake .
  make
done

cd ../

rm -fr ../lib/* ../include/*


# TODO add branch verification

mkdir ../include/glad ../include/glfw ../include/gml ../include/json

mv glad/libglad.a ../lib
mv glad/include/glad/glad.h ../include/glad/

mv glfw/src/libglfw3.a ../lib
mv glfw/include/glfw3.h ../include/glfw

mv glm/glm/libglm_static.a ../lib
mv glm/glm/*.hpp ../include/gml

mv jsoncpp/lib/libjsoncpp.a ../lib
mv jsoncpp/include/json/*.h ../include/json
