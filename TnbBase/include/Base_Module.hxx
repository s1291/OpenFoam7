#pragma once
#ifndef _Base_Module_Header
#define _Base_Module_Header

#ifdef FoamBase_STATIC_DEFINE
#define FoamBase_EXPORT
#else
#ifdef FoamBase_EXPORT_DEFINE
#define FoamBase_EXPORT __declspec(dllexport)
#else
#define FoamBase_EXPORT __declspec(dllimport)
#endif // FoamBase_EXPORT_DEFINE

#endif // FoamBase_STATIC_DEFINE

#ifdef Foam_EXPORT_DEFINE
#define Foam_EXPORT FoamBase_EXPORT
#else
#define Foam_EXPORT 
#endif // Foam_EXPORT_DEFINE


#endif // !_Base_Module_Header
