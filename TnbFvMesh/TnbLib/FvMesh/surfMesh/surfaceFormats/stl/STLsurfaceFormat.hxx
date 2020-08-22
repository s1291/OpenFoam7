#pragma once
#ifndef _STLsurfaceFormat_Header
#define _STLsurfaceFormat_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
	\\  /    A nd           | Copyright (C) 2011-2019 OpenFOAM Foundation
	 \\/     M anipulation  |
-------------------------------------------------------------------------------
License
	This file is part of OpenFOAM.

	OpenFOAM is free software: you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
	for more details.

	You should have received a copy of the GNU General Public License
	along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
	tnbLib::fileFormats::STLsurfaceFormat

Description
	Provide a means of reading/writing STL files (ASCII and binary).

Note
	For efficiency, the zones are sorted before creating the faces.
	The class is thus derived from MeshedSurface.

SourceFiles
	STLsurfaceFormat.C
	STLsurfaceFormatASCII.L

\*---------------------------------------------------------------------------*/

#include <STLsurfaceFormatCore.hxx>
#include <MeshedSurface.hxx>
#include <MeshedSurfaceProxy.hxx>
#include <UnsortedMeshedSurface.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace fileFormats
	{

		/*---------------------------------------------------------------------------*\
							 Class STLsurfaceFormat Declaration
		\*---------------------------------------------------------------------------*/

		template<class Face>
		class STLsurfaceFormat
			:
			public MeshedSurface<Face>
		{
			// Private Member Functions

				//- Write Face (ASCII)
			static inline void writeShell
			(
				Ostream&,
				const pointField&,
				const Face&
			);

			//- Write Face (BINARY)
			static inline void writeShell
			(
				ostream&,
				const pointField&,
				const Face&,
				const label zoneI
			);


		public:

			// Constructors

				//- Construct from file name
			STLsurfaceFormat(const fileName&);

			//- Disallow default bitwise copy construction
			STLsurfaceFormat(const STLsurfaceFormat<Face>&) = delete;


			// Selectors

				//- Read file and return surface
			static autoPtr<MeshedSurface<Face>> New(const fileName& name)
			{
				return autoPtr<MeshedSurface<Face>>
					(
						new STLsurfaceFormat<Face>(name)
						);
			}


			//- Destructor
			virtual ~STLsurfaceFormat()
			{}


			// Member Functions

				//- Write surface mesh components by proxy (as ASCII)
			static void writeAscii
			(
				const fileName&,
				const MeshedSurfaceProxy<Face>&
			);

			//- Write surface mesh components by proxy (as BINARY)
			static void writeBinary
			(
				const fileName&,
				const MeshedSurfaceProxy<Face>&
			);

			//- Write surface mesh components by proxy
			//  as ASCII or BINARY, depending on the extension
			static void write(const fileName&, const MeshedSurfaceProxy<Face>&);

			//- Write UnsortedMeshedSurface (as ASCII) sorted by zone
			static void writeAscii
			(
				const fileName&,
				const UnsortedMeshedSurface<Face>&
			);

			//- Write UnsortedMeshedSurface (as BINARY) unsorted by zone
			static void writeBinary
			(
				const fileName&,
				const UnsortedMeshedSurface<Face>&
			);

			//- Write UnsortedMeshedSurface
			//  as ASCII or BINARY, depending on the extension
			static void write(const fileName&, const UnsortedMeshedSurface<Face>&);

			//- Read from file
			virtual bool read(const fileName&);

			//- Write object
			virtual void write(const fileName& name) const
			{
				write(name, MeshedSurfaceProxy<Face>(*this));
			}


			// Member Operators

				//- Disallow default bitwise assignment
			void operator=(const STLsurfaceFormat<Face>&) = delete;
		};


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace fileFormats
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#include <STLsurfaceFormat.cxx>
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_STLsurfaceFormat_Header
