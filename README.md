# Anisotropic Kuwahara Filter

[Based on this research paper](https://www.researchgate.net/publication/220507613_Image_and_Video_Abstraction_by_Anisotropic_Kuwahara_Filtering)

This project allows to apply the Kuwahara anisotropic filter on a stream of camera or images given in input of the program.
This project uses the OpenCV library.
There are two versions of the code. One version uses OpenGL to have real-time processing.
CMake is the build system of the project.
## Build from source
```bash
cmake .
make -j8
```

## Usage
Run the shell
```bash
# Version without OpenGL
./src/main [Optional: Image paths]

# Version with OpenGL
./src/main_opengl [Optional: Image paths]
```

# OpenGL version
Use space bar to activate or desactivate the filter.