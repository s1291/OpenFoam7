#pragma once
#ifndef _noScatter_Header
#define _noScatter_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
	\\  /    A nd           | Copyright (C) 2012-2019 OpenFOAM Foundation
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
	tnbLib::radiationModels::scatterModels::noScatter

Description
	Dummy scatter model for 'none'

SourceFiles
	scatterModel.C

\*---------------------------------------------------------------------------*/

#include <scatterModel.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace radiationModels
	{
		namespace scatterModels
		{

			/*---------------------------------------------------------------------------*\
									  Class noScatter Declaration
			\*---------------------------------------------------------------------------*/

			class noScatter
				:
				public scatterModel
			{

			public:

				//- Runtime type information
				TypeName("none");


				// Constructors

					//- Construct from components
				noScatter(const dictionary& dict, const fvMesh& mesh);


				//- Destructor
				virtual ~noScatter();


				// Member Functions

					//- Return scatter coefficient
				tmp<volScalarField> sigmaEff() const;
			};


			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		} // End namespace scatterModels
	} // End namespace radiationModels
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_noScatter_Header
