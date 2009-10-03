

#ifndef SORE_DLLDEFINES_H
#define SORE_DLLDEFINES_H

#if defined (_WIN32) 
	#if defined(sore_lib_EXPORTS)
		#define SORE_EXPORT __declspec(dllexport)
	#else
		#define SORE_EXPORT __declspec(dllimport)
	#endif
#else
	#define SORE_EXPORT
#endif

#endif
