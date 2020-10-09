#pragma once
#ifndef _writeCellCentres_Header
#define _writeCellCentres_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
	\\  /    A nd           | Copyright (C) 2016-2019 OpenFOAM Foundation
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
	tnbLib::functionObjects::writeCellCentres

Description
	Writes the cell-centres volVectorField and the three component fields as
	volScalarFields.

	Example of function object specification:
	\verbatim
	writeCellCentres
	{
		type        writeCellCentres;
		libs        ("libfieldFunctionObjects.so");
		...
	}
	\endverbatim

Usage
	\table
		Property  | Description                 | Required    | Default value
		type      | type name: writeCellCentres | yes       |
	\endtable

See also
	tnbLib::functionObjects::fvMeshFunctionObject

SourceFiles
	writeCellCentres.C

\*---------------------------------------------------------------------------*/

#include <fvMeshFunctionObject.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace functionObjects
	{

		/*---------------------------------------------------------------------------*\
							   Class writeCellCentres Declaration
		\*---------------------------------------------------------------------------*/

		class writeCellCentres
			:
			public fvMeshFunctionObject
		{
		public:

			//- Runtime type information
			TypeName("writeCellCentres");


			// Constructors

				//- Construct from Time and dictionary
			writeCellCentres
			(
				const word& name,
				const Time& runTime,
				const dictionary& dict
			);

			//- Disallow default bitwise copy construction
			writeCellCentres(const writeCellCentres&) = delete;


			//- Destructor
			virtual ~writeCellCentres();


			// Member Functions

				//- Do nothing
			virtual bool execute();

			//- Write the cell-centre fields
			virtual bool write();


			// Member Operators

				//- Disallow default bitwise assignment
			void operator=(const writeCellCentres&) = delete;
		};


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace functionObjects
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_writeCellCentres_Header
