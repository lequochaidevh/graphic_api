
### BUILD:

```sh
export WORKING_DIR=.
export SOURCE_CPP=$(find . \( -path "./build" -o -path "./src/vendor/glm" \) -prune -o -name "*.cpp" -print | xargs)
```

```sh
./cmakeBuild_and_Run.sh -p "src/*.cpp"
```

### BUILD, RUN AND CLEAN
```sh
./cmakeBuild_and_Run.sh -p "src/*.cpp" -c
```

