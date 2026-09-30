# Rastertek's DirectX 11 Tutorials, Revisited

## Introduction

Rastertek's [DX11 tutorials](https://www.rastertek.com) DirectX11 tutorials are still an extremely useful resource for learning DirectX 11, even if they are a little dated now. The aim of this repo is to revisit the source code from the tutorials and modernise it. This does not supplant the original tutorials and you still need to follow them to understand what's going on (seriously, those tutorials are great).

## Building

You need Visual Studio 2026 with the _Desktop development with C++_ and _Game development with C++_ workloads installed. I work with the preview version of MSVC (to pick up issues early), so you'll also need the _MSVC Build Tools for x64/x86 (Preview)_ Visual Studio component installed, but this is not strictly necessary: you can disable the use of the preview tools in the individual project settings.

## Status

### DirectX 11 on Windows 10

The first five tutorials (sans the first setting up one) from this series have been updated.

* [02. Creating a Framework and Window](<src/DirectX11OnWindows10/02. Creating a Framework and Window>)
* [03. Initializing DirectX 11](<src/DirectX11OnWindows10/03. Initializing DirectX 11>)
* [04. Buffers, Shaders, and HLSL](<src/DirectX11OnWindows10/04. Buffers, Shaders, and HLSL>)
* [05. Texturing](<src/DirectX11OnWindows10/05. Texturing>)
* [06. Diffuse Lighting](<src/DirectX11OnWindows10/06. Diffuse Lighting>)

## Changes

The following changes have been done. This is just a running list, and more is planned.

* Converted the code to inline C++20 modules. The traditional .h/.cpp distinction is gone, this reduces the total files, LoC and improves code locality. There is one module `demo`, which exports partitions, and each partition is named after the `.h`/`.cpp` file pair it replaces.
* Unnecessary use of dynamic memory allocations (e.g. in `main()`) have been removed in favour of either scoped lifecycles or managed containers like `std::vector`. More work remains to be done here.
* Functions have been converted to trailing return type syntax.
* Various constants have been made `constexpr`.
* Replaced various raw C string arrays and their related functions with `std::string`/`std::wstring` and their related types.
* The two-phase initialisation pattern will be removed with a combination of `std::optional` and making constructors do proper initialization. This is not yet done and is reliant on a few other changes first (like making the destructors actually tear down the object).
* Vertex and pixel shader files have been given the `.hlsl` extension to allow proper syntax highlighting in Visual Studio (they have been disabled in the auto build process, as the samples compile them at runtime).
* The directories have been flattened. So far, the samples are too small and self-contained to make additional subdirectories necessary.
* Converted code to use _Almost Always Auto_ idiom.
* The idiosyncratic use of `return` statements at the end of void functions has been removed.
* Initialisation of class member variables (e.g. to null out pointers) is now done inline, which removes the need for default constructors.
