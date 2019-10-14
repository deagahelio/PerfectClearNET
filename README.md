# PerfectClearNET
[![NuGet - Download](https://img.shields.io/badge/nuget-download-orange)](https://www.nuget.org/packages/PerfectClearNET/)
[![Discord - Join Chat](https://img.shields.io/badge/discord-join%20chat-blue)](https://discord.gg/vfrmzUV)

.NET version of the knewjade Perfect Clear solution finder (via DLL importing).

## Installation and Usage Example

Use PerfectClearNET from [NuGet](https://www.nuget.org/packages/MisaMinoNET/) in your project.

```cs
using PerfectClearNET;

// Listen for search completion
PerfectClear.Finished += ...;

// Start search in the background
PerfectClear.Find(...);

// Abort search prematurely
PerfectClear.Abort();

// Access results of last search
PerfectClear.LastSolution;
PerfectClear.LastTime;
```

A common need with the Perfect Clear Finder is a pathfinder to tell how to move the piece into position, [MisaMinoNET](https://github.com/mat1jaczyyy/MisaMinoNET)'s pathfinder can be used for this purpose:

```cs
using PerfectClearNET;
using MisaMinoNET;

// Utilize MisaMinoNET pathfinder after search
movements = MisaMino.FindPath(
    ...,
    PerfectClear.LastSolution[0].Piece,
    PerfectClear.LastSolution[0].X,
    PerfectClear.LastSolution[0].Y,
    PerfectClear.LastSolution[0].R,
    current_piece != PerfectClear.LastSolution[0].Piece,
    ...
);
```
