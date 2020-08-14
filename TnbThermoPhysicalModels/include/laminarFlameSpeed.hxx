#pragma once
#ifndef _laminarFlameSpeed_Header
#define _laminarFlameSpeed_Header

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

Namespace
	tnbLib::laminarFlameSpeedModels

Description
	Namespace for laminar flame speed models


Class
	tnbLib::laminarFlameSpeed

Description
	Abstract class for laminar flame speed

SourceFiles
	laminarFlameSpeed.C
	laminarFlameSpeedNew.C

\*---------------------------------------------------------------------------*/

#include <psiuReactionThermo.hxx>
#include <autoPtr.hxx>
#include <runTimeSelectionTables.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							   Class laminarFlameSpeed Declaration
	\*---------------------------------------------------------------------------*/

	class laminarFlameSpeed
	{

	protected:

		// Protected data

		const psiuReactionThermo& psiuReactionThermo_;

		//- Fuel
		word fuel_;

		//- Equivalence ratio of a homogeneous mixture
		scalar equivalenceRatio_;


	private:

		// Private Member Functions

			//- Construct as copy (not implemented)
		laminarFlameSpeed(const laminarFlameSpeed&);
		void operator=(const laminarFlameSpeed&);


	public:

		//- Runtime type information
		TypeName("laminarFlameSpeed");


		// Declare run-time constructor selection table

		declareRunTimeSelectionTable
		(
			autoPtr,
			laminarFlameSpeed,
			dictionary,
			(
				const dictionary& dict,
				const psiuReactionThermo& ct
				),
				(dict, ct)
		);


		// Constructors

			//- Construct from dictionary and psiuReactionThermo
		laminarFlameSpeed
		(
			const dictionary&,
			const psiuReactionThermo&
		);


		// Selector

		static autoPtr<laminarFlameSpeed> New
		(
			const psiuReactionThermo&
		);


		//- Destructor
		virtual ~laminarFlameSpeed();


		// Member Functions

			//- Return the laminar flame speed [m/s]
		virtual tmp<volScalarField> operator()() const = 0;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_laminarFlameSpeed_Header
