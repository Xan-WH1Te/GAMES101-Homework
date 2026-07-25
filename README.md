# GAMES101 Homework

> GAMES101: Introduction to Computer Graphics — 现代计算机图形学入门
>
> 闫令琪, 2020 Spring

My homework implementations for [GAMES101](https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html).

## Environment

- **OS:** Ubuntu 24.04 (WSL2 on Windows 11)
- **Compiler:** GCC 13.3
- **Build:** CMake
- **Libraries:** Eigen, OpenCV

## Structure

| # | Topic | Status |
|---|-------|--------|
| PA0 | Environment Setup & Eigen Basics | ✅ |
| PA1 | Rasterization | ✅ |
| PA2 | Z-Buffering | ✅ |
| PA3 | Shading | ⬜ |
| PA4 | Bézier Curves | ⬜ |
| PA5 | Ray Tracing | ⬜ |
| PA6 | BVH Acceleration | ⬜ |
| PA7 | Path Tracing | ⬜ |
| PA8 | Mass-Spring Animation | ⬜ |

## Build

Each assignment directory has its own CMakeLists.txt. To build:

```bash
cd PA<N>
mkdir build && cd build
cmake ..
make
./<executable>
```
