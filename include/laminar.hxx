#pragma once
#ifndef _laminar_Header
#define _laminar_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
	\\  /    A nd           | Copyright (C) 2013-2019 OpenFOAM Foundation
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
	tnbLib::regionModels::surfaceFilmModels::laminar

Description
	Film laminar turbulence model.

SourceFiles
	laminar.C

\*---------------------------------------------------------------------------*/

#include <filmTurbulenceModel.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace regionModels
	{
		namespace surfaceFilmModels
		{

			/*---------------------------------------------------------------------------*\
									   Class laminar Declaration
			\*---------------------------------------------------------------------------*/

			class laminar
				:
				public filmTurbulenceModel
			{
				// Private Data

					//- Surface roughness coefficient
				scalar Cf_;


			public:

				//- Runtime type information
				TypeName("laminar");


				// Constructors

					//- Construct from surface film model
				laminar(surfaceFilmRegionModel& film, const dictionary& dict);

				//- Disallow default bitwise copy construction
				laminar(const laminar&) = delete;


				//- Destructor
				virtual ~laminar();


				// Member Functions

					// Evolution

						//- Return the film surface velocity
				virtual tmp<volVectorField> Us() const;

				//- Return the film turbulence viscosity
				virtual tmp<volScalarField> mut() const;

				//- Correct/update the model
				virtual void correct();

				//- Return the source for the film momentum equation
				virtual tmp<fvVectorMatrix> Su(volVectorField& U) const;


				// Member Operators

					//- Disallow default bitwise assignment
				void operator=(const laminar&) = delete;
			};


			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		} // End namespace surfaceFilmModels
	} // End namespace regionModels
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_laminar_Header
