# Mesh Viewer for Triangular Surfaces

The program is used to load meshes and render using OpenGL.

## Usage

Open the .sln file using Visual Studio, then change Qt version and include path of OpenMesh, and finally compile.

### Supported File Formats

.obj .off .ply .stl

## Environment & Compiler

### Windows 10

### [Visual Studio 2017](https://visualstudio.microsoft.com/vs/)

Recommended version: the latest, 15.9.5 (19.16.27026) (at Jan. 2019)

Debug and Release configuration in x64 environment.

> The complier need to support C++11 features and some C++14 features.

## Required External Library

### [Qt](https://download.qt.io)

Recommended version: 5.11.3 msvc2017_64

> Qt 5.4.0 introduces a new QOpenGLWidget that replaces the old QGLWidget.
>
> Warning: Before Qt 5.6.0, the screenshot feature has a bug due to the function `grabFramebuffer()`. (refer to https://bugreports.qt.io/browse/QTBUG-48450)
>
> Warning: Before Qt 5.12.0, the screenshot with multisampling does not generate an image with a transparent background.
>
> Warning: Qt 5.12.0 has a bug on drag and drop event. (refer to https://bugreports.qt.io/browse/QTBUG-72420)

### [OpenMesh](http://openmesh.org/download/)

Recommended version: the latest, 7.1 (at Jan. 2019)
