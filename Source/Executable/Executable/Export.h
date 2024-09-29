#ifndef EXPORT_H
#define EXPORT_H

// Define macros for exporting/importing symbols
#ifdef BUILDING_DLL // Defined when building the DLL
#define GAME_API __declspec(dllexport)
#else // Defined when using the DLL
#define GAME_API __declspec(dllimport)
#endif

#ifdef BUILDING_EXE // Defined when building the EXE
#define SHIPYARD_API __declspec(dllimport)
#else // Defined when using the EXE
#define SHIPYARD_API __declspec(dllexport)
#endif

#endif // EXPORT_H
