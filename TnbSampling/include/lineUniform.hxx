#pragma once
#ifndef _lineUniform_Header
#define _lineUniform_Header

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
	tnbLib::sampledSets::lineUniform

Description
	Uniform samples along a line

Usage
	\table
		Property    | Description                            | Req'd? | Default
		start       | The start point of the line            | yes    |
		end         | The end point of the line              | yes    |
		nPoints     | The number of points                   | yes    |
		axis        | The coordinate axis that is written    | yes    |
	\endtable

	Example specification:
	\verbatim
	{
		type        lineUniform;
		start       (0.65 0.3 0.3);
		end         (0.65 -0.3 -0.1);
		nPoints     200;
		axis        x;
	}
	\endverbatim

SourceFiles
	lineUniform.C

\*---------------------------------------------------------------------------*/

#include <passiveParticleCloud.hxx>
#include <sampledSet.hxx>
#include <DynamicList.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace sampledSets
	{

		/*---------------------------------------------------------------------------*\
								   Class lineUniform Declaration
		\*---------------------------------------------------------------------------*/

		class lineUniform
			:
			public sampledSet
		{
			// Private Data

				//- Starting point
			const point start_;

			//- End point
			const point end_;

			//- Number of points
			const label nPoints_;


			// Private Member Functions

				//- Samples from start_ to end_. samplingSegments contains segmentNo
				//  for each sample.
			void calcSamples
			(
				DynamicList<point>& samplingPts,
				DynamicList<label>& samplingCells,
				DynamicList<label>& samplingFaces,
				DynamicList<label>& samplingSegments,
				DynamicList<scalar>& samplingCurveDist
			) const;

			//- Uses calcSamples to obtain samples. Copies them into *this.
			void genSamples();


		public:

			//- Runtime type information
			TypeName("lineUniform");


			// Constructors

				//- Construct from dictionary
			lineUniform
			(
				const word& name,
				const polyMesh& mesh,
				const meshSearch& searchEngine,
				const dictionary& dict
			);


			//- Destructor
			virtual ~lineUniform();
		};


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace sampledSets
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_lineUniform_Header
