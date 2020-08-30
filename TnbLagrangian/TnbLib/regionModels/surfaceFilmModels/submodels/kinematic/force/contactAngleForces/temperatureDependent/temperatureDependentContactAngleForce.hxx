#pragma once
#ifndef _temperatureDependentContactAngleForce_Header
#define _temperatureDependentContactAngleForce_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
	\\  /    A nd           | Copyright (C) 2017-2019 OpenFOAM Foundation
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
	tnbLib::regionModels::surfaceFilmModels::temperatureDependentContactAngleForce

Description
	Temperature dependent contact angle force

	The contact angle in degrees is specified as a tnbLib::Function1 type, to
	enable the use of, e.g.  constant, polynomial, table values.

See also
	tnbLib::regionModels::surfaceFilmModels::contactAngleForce
	tnbLib::Function1Types

SourceFiles
	temperatureDependentContactAngleForce.C

\*---------------------------------------------------------------------------*/

#include <contactAngleForce.hxx>
#include <Function1.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace regionModels
	{
		namespace surfaceFilmModels
		{

			/*---------------------------------------------------------------------------*\
						Class temperatureDependentContactAngleForce Declaration
			\*---------------------------------------------------------------------------*/

			class temperatureDependentContactAngleForce
				:
				public contactAngleForce
			{
				// Private Data

					//- Contact angle function
				autoPtr<Function1<scalar>> thetaPtr_;


			protected:

				//- Return the contact angle field
				virtual tmp<volScalarField> theta() const;


			public:

				//- Runtime type information
				TypeName("temperatureDependentContactAngle");


				// Constructors

					//- Construct from surface film model
				temperatureDependentContactAngleForce
				(
					surfaceFilmRegionModel& film,
					const dictionary& dict
				);

				//- Disallow default bitwise copy construction
				temperatureDependentContactAngleForce
				(
					const temperatureDependentContactAngleForce&
				);


				//- Destructor
				virtual ~temperatureDependentContactAngleForce();


				// Member Operators

					//- Disallow default bitwise assignment
				void operator=(const temperatureDependentContactAngleForce&) = delete;
			};


			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		} // End namespace surfaceFilmModels
	} // End namespace regionModels
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_temperatureDependentContactAngleForce_Header
