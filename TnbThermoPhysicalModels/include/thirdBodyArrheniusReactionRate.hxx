#pragma once
#ifndef _thirdBodyArrheniusReactionRate_Header
#define _thirdBodyArrheniusReactionRate_Header

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
	tnbLib::thirdBodyArrheniusReactionRate

Description
	Arrhenius reaction rate enhanced by third-body interaction.

SourceFiles
	thirdBodyArrheniusReactionRateI.H

\*---------------------------------------------------------------------------*/

#include <ArrheniusReactionRate.hxx>
#include <thirdBodyEfficiencies.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// Forward declaration of friend functions and operators

	class thirdBodyArrheniusReactionRate;

	Ostream& operator<<(Ostream&, const thirdBodyArrheniusReactionRate&);


	/*---------------------------------------------------------------------------*\
				  Class thirdBodyArrheniusReactionRate Declaration
	\*---------------------------------------------------------------------------*/

	class thirdBodyArrheniusReactionRate
		:
		ArrheniusReactionRate
	{
		// Private Data

		thirdBodyEfficiencies thirdBodyEfficiencies_;
		List<Tuple2<label, scalar>> beta_;


	public:

		// Constructors

			//- Construct from components
		inline thirdBodyArrheniusReactionRate
		(
			const scalar A,
			const scalar beta,
			const scalar Ta,
			const thirdBodyEfficiencies& tbes
		);

		//- Construct from dictionary
		inline thirdBodyArrheniusReactionRate
		(
			const speciesTable& species,
			const dictionary& dict
		);


		// Member Functions

			//- Return the type name
		static word type()
		{
			return "thirdBodyArrhenius";
		}

		inline scalar operator()
			(
				const scalar p,
				const scalar T,
				const scalarField& c
				) const;

		inline scalar ddT
		(
			const scalar p,
			const scalar T,
			const scalarField& c
		) const;

		//- Third-body efficiencies (beta = 1-alpha)
		//  non-empty only for third-body reactions
		//  with enhanced molecularity (alpha != 1)
		inline const List<Tuple2<label, scalar>>& beta() const
		{
			return beta_;
		}

		//- Species concentration derivative of the pressure dependent term
		inline void dcidc
		(
			const scalar p,
			const scalar T,
			const scalarField& c,
			scalarField& dcidc
		) const;

		//- Temperature derivative of the pressure dependent term
		inline scalar dcidT
		(
			const scalar p,
			const scalar T,
			const scalarField& c
		) const;

		//- Write to stream
		inline void write(Ostream& os) const;


		// Ostream Operator

		inline friend Ostream& operator<<
			(
				Ostream&,
				const thirdBodyArrheniusReactionRate&
				);
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <thirdBodyArrheniusReactionRateI.hxx>

#endif // !_thirdBodyArrheniusReactionRate_Header
