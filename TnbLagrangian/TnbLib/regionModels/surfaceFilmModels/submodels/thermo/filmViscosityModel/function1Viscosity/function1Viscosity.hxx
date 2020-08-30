#pragma once
#ifndef _function1Viscosity_Header
#define _function1Viscosity_Header

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
	tnbLib::regionModels::surfaceFilmModels::function1Viscosity

Description
	The tnbLib::Function1 temperature-dependent viscosity model multiplies the
	viscosity of a base model by a Function1 temperature function.

SourceFiles
	function1Viscosity.C

\*---------------------------------------------------------------------------*/

#include <filmViscosityModel.hxx>
#include <Function1.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace regionModels
	{
		namespace surfaceFilmModels
		{

			/*---------------------------------------------------------------------------*\
								Class function1Viscosity Declaration
			\*---------------------------------------------------------------------------*/

			class function1Viscosity
				:
				public filmViscosityModel
			{
				// Private Member Functions

					//- Disallow default bitwise copy construction
				function1Viscosity(const function1Viscosity&) = delete;

				//- Disallow default bitwise assignment
				void operator=(const function1Viscosity&) = delete;


			protected:

				// Protected data

					//- Base viscosity model
				autoPtr<filmViscosityModel> viscosity_;

				//- Viscosity factor as a function of temperature
				autoPtr<Function1<scalar>> function_;


			public:

				//- Runtime type information
				TypeName("function1");


				// Constructors

					//- Construct from surface film model
				function1Viscosity
				(
					surfaceFilmRegionModel& film,
					const dictionary& dict,
					volScalarField& mu
				);


				//- Destructor
				virtual ~function1Viscosity();


				// Member Functions

					//- Correct
				virtual void correct
				(
					const volScalarField& p,
					const volScalarField& T
				);
			};


			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		} // End namespace surfaceFilmModels
	} // End namespace regionModels
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_function1Viscosity_Header
