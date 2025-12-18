# Rotating Cube – Software Graphics Pipeline in C

## Overview

This project is a **from-scratch implementation of a complete software graphics pipeline in C**, rendering a **rotating 3D cube** in real time.  
No external graphics APIs (OpenGL, Vulkan, DirectX) are used — all transformations, projections, rasterization, and timing are implemented manually.

The goal of this project is **educational and technical**: to deeply understand how modern 3D graphics work internally by rebuilding the pipeline step by step in plain C.

---

## Features

- Fully custom **software graphics pipeline**
- 3D **model → world → view → projection** transformations
- **Perspective projection**
- Real-time **rotating cube**
- Manual **rasterization**
- Custom **math library** (vectors, matrices)
- Frame timing and animation
- Written in **ISO C (gnu17)**
- Runs on **Windows**, **Linux (X11)** and **Wine** using GCC

---

## Requirements

To build, run, and debug this project, the following tools are required:

- **gcc**  
GNU Compiler Collection (MinGW-w64 recommended)

- **make**  
Used to build the project via the provided Makefile

- **gdb**  
GNU Debugger (optional but recommended for development and debugging)

- **Windows 10 / 11** or **Linux with X11** or **Linux with Wine**

---

## Graphics Pipeline Stages

The project implements the following pipeline stages:

1. Model Space  
2. World Transformation  
3. View Transformation  
4. Projection  
5. Clipping & Normalization  
6. Viewport Transform  
7. Rasterization  
8. Presentation

---

## Project Structure

```text
Gui_WSEM_Cube/
│
├── inc/                # libraries needed (Header-Based)
│   ├── Lib3D_Cube.h
│   ├── Lib3D_Mathlib.h
│   ├── Lib3D_Mesh.h
│
├── src/                # Program entry point and library binding
│   ├── Main.c          
│
├── build/              # Build output
│   └── Main.exe
│
├── LICENCE             # GPL 3.0 Licence
│
├── Makefile.linux      # make file for build on Linux (X11)
├── Makefile.windows    # make file for build on Windows
├── Makefile.wine       # make file for build on Linux (Wine)
└── README.md           # information about project
```

## Installation

How to install and set up your project:

```bash
# Example installation commands
git clone https://github.com/codeleaded/Gui_WSEM_Cube.git
cd Gui_WSEM_Cube
# just build
make all
# build and run
make do
# run after build
make exe
# clean build
make clean
```