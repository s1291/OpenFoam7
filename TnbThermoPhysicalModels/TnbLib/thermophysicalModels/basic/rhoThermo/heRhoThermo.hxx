#pragma once
#ifndef _heRhoThermo_Header
#define _heRhoThermo_Header

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
	tnbLib::heRhoThermo

Description
	Energy for a mixture based on density

SourceFiles
	heRhoThermo.C

\*---------------------------------------------------------------------------*/

#include <rhoThermo.hxx>
#include <heThermo.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							 Class heRhoThermo Declaration
	\*---------------------------------------------------------------------------*/

	template<class BasicPsiThermo, class MixtureType>
	class heRhoThermo
		:
		public heThermo<BasicPsiThermo, MixtureType>
	{
		// Private Member Functions

			//- Calculate the thermo variables
		void calculate();

		//- Construct as copy (not implemented)
		heRhoThermo(const heRhoThermo<BasicPsiThermo, MixtureType>&);


	public:

		//- Runtime type information
		TypeName("heRhoThermo");


		// Constructors

			//- Construct from mesh and phase name
		heRhoThermo
		(
			const fvMesh&,
			const word& phaseName
		);


		//- Destructor
		virtual ~heRhoThermo();


		// Member Functions

			//- Update properties
		virtual void correct();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <heRhoThermoI.hxx>

//#ifdef NoRepository
//#include <heRhoThermo.cxx>
//#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_heRhoThermo_Header
