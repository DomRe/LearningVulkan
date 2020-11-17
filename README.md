# Vulkano
C++20 Vulkan renderer built for learning purposes.

### Limitations:
- Only supports 1 GPU.
- Does not support Integrated GPUs.

### Building

Only ```x64``` architecture and ```Windows 10``` are supported.

Step by Step:

```
Install requirements:
1. Visual Studio 2019 16.9 preview 1.
    - Desktop development with C++
    - Game development with C++
2. Latest Windows 10 SDK (10.0.19041.0 min).
3. Vulkan SDK from LunarG.
```
```
Building:
1. Clone repo and submodules: git clone --recursive https://github.com/DomRe/Vulkano.git
2. Update header and library directories to match the path of the installed Vulkan SDK.
3. Select build mode: Debug or Release.
4. Build and Run.
```

### License:
See [License.txt](https://github.com/DomRe/Vulkano/blob/master/LICENSE.txt).