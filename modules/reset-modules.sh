#!/bin/bash

git submodule foreach git reset --hard

for d in *
do
  [[ -d "${d}" ]] || continue
  cd $d
  git clean -fd
  cd ..
done