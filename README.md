# OasisViewer

The goal of this project is to create a computational geometry viewer using the Oasis file format. It is a work in progress.

It is capable of creating and viewing files that contain simple layouts with a few compatible geometries. Files produced are readable by KLayout https://www.klayout.de/. Sample files are included in the data directory.

If you want to produce different files, you will need to modify createOasisFile() in main.cpp as this project is not capable of editing geometries. There are currently some issues and restrictions on what kind of geometries that can be successful written or read.
