#pragma once
#ifndef _cloud_Header
#define _cloud_Header

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
	tnbLib::cloud

Description
	A cloud is a collection of lagrangian particles

SourceFiles
	cloud.C

\*---------------------------------------------------------------------------*/

#include <objectRegistry.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// Forward declaration of classes
	class mapPolyMesh;

	/*---------------------------------------------------------------------------*\
								Class cloud Declaration
	\*---------------------------------------------------------------------------*/

	class cloud
		:
		public objectRegistry
	{

	public:

		//- Runtime type information
		TypeName("cloud");

		//- The prefix to local: %lagrangian
		static const word prefix;

		//- The default cloud name: %defaultCloud
		static word defaultName;


		// Constructors

			//- Construct for the given objectRegistry and named cloud instance
		cloud(const objectRegistry&, const word& cloudName = defaultName);

		//- Disallow default bitwise copy construction
		cloud(const cloud&) = delete;


		//- Destructor
		virtual ~cloud();


		// Member Functions

			// Edit

				//- Remap the cells of particles corresponding to the
				//  mesh topology change
		virtual void autoMap(const mapPolyMesh&);


		// Member Operators

			//- Disallow default bitwise assignment
		void operator=(const cloud&) = delete;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_cloud_Header
