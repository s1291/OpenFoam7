#pragma once
#ifndef _filmThermoModel_Header
#define _filmThermoModel_Header

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
	tnbLib::regionModels::surfaceFilmModels::filmThermoModel

Description
	Base class for film thermo models

SourceFiles
	filmThermoModel.C
	filmThermoModelNew.C

\*---------------------------------------------------------------------------*/

#include <filmSubModelBase.hxx>
#include <runTimeSelectionTables.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace regionModels
	{
		namespace surfaceFilmModels
		{

			/*---------------------------------------------------------------------------*\
								   Class filmThermoModel Declaration
			\*---------------------------------------------------------------------------*/

			class filmThermoModel
				:
				public filmSubModelBase
			{
			public:

				//- Runtime type information
				TypeName("filmThermoModel");


				// Declare runtime constructor selection table

				declareRunTimeSelectionTable
				(
					autoPtr,
					filmThermoModel,
					dictionary,
					(
						surfaceFilmRegionModel& film,
						const dictionary& dict
						),
						(film, dict)
				);

				// Constructors

					//- Construct null
				filmThermoModel(surfaceFilmRegionModel& film);

				//- Construct from type name, dictionary and surface film model
				filmThermoModel
				(
					const word& modelType,
					surfaceFilmRegionModel& film,
					const dictionary& dict
				);

				//- Disallow default bitwise copy construction
				filmThermoModel(const filmThermoModel&) = delete;


				// Selectors

					//- Return a reference to the selected phase change model
				static autoPtr<filmThermoModel> New
				(
					surfaceFilmRegionModel& film,
					const dictionary& dict
				);


				//- Destructor
				virtual ~filmThermoModel();


				// Member Functions

					//- Return the specie name
				virtual const word& name() const = 0;


				// Elemental access

					//- Return density [kg/m^3]
				virtual scalar rho(const scalar p, const scalar T) const = 0;

				//- Return dynamic viscosity [Pa.s]
				virtual scalar mu(const scalar p, const scalar T) const = 0;

				//- Return surface tension [kg/s^2]
				virtual scalar sigma(const scalar p, const scalar T) const = 0;

				//- Return specific heat capacity [J/kg/K]
				virtual scalar Cp(const scalar p, const scalar T) const = 0;

				//- Return thermal conductivity [W/m/K]
				virtual scalar kappa(const scalar p, const scalar T) const = 0;

				//- Return diffusivity [m2/s]
				virtual scalar D(const scalar p, const scalar T) const = 0;

				//- Return latent heat [J/kg]
				virtual scalar hl(const scalar p, const scalar T) const = 0;

				//- Return vapour pressure [Pa]
				virtual scalar pv(const scalar p, const scalar T) const = 0;

				//- Return molecular weight [kg/kmol]
				virtual scalar W() const = 0;

				//- Return boiling temperature [K]
				virtual scalar Tb(const scalar p) const = 0;


				// Field access

					//- Return density [kg/m^3]
				virtual tmp<volScalarField> rho() const = 0;

				//- Return dynamic viscosity [Pa.s]
				virtual tmp<volScalarField> mu() const = 0;

				//- Return surface tension [kg/s^2]
				virtual tmp<volScalarField> sigma() const = 0;

				//- Return specific heat capacity [J/kg/K]
				virtual tmp<volScalarField> Cp() const = 0;

				//- Return thermal conductivity [W/m/K]
				virtual tmp<volScalarField> kappa() const = 0;


				// Member Operators

					//- Disallow default bitwise assignment
				void operator=(const filmThermoModel&) = delete;
			};


			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		} // End namespace surfaceFilmModels
	} // End namespace regionModels
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_filmThermoModel_Header
