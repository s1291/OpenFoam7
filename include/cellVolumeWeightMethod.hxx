#pragma once
#ifndef _cellVolumeWeightMethod_Header
#define _cellVolumeWeightMethod_Header

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
	tnbLib::cellVolumeWeightMethod

Description
	Cell-volume-weighted mesh-to-mesh interpolation class

	Volume conservative.

SourceFiles
	cellVolumeWeightMethod.C

\*---------------------------------------------------------------------------*/

#include <meshToMeshMethod.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
					   Class cellVolumeWeightMethod Declaration
	\*---------------------------------------------------------------------------*/

	class cellVolumeWeightMethod
		:
		public meshToMeshMethod
	{
	protected:

		// Protected Member Functions

			//- Find indices of overlapping cells in src and tgt meshes - returns
			//  true if found a matching pair
		bool findInitialSeeds
		(
			const labelList& srcCellIDs,
			const boolList& mapFlag,
			const label startSeedI,
			label& srcSeedI,
			label& tgtSeedI
		) const;

		//- Calculate the mesh-to-mesh addressing and weights
		void calculateAddressing
		(
			labelListList& srcToTgtCellAddr,
			scalarListList& srcToTgtCellWght,
			labelListList& tgtToSrcCellAddr,
			scalarListList& tgtToSrcCellWght,
			const label srcSeedI,
			const label tgtSeedI,
			const labelList& srcCellIDs,
			boolList& mapFlag,
			label& startSeedI
		);

		//- Set the next cells in the advancing front algorithm
		void setNextCells
		(
			label& startSeedI,
			label& srcCelli,
			label& tgtCelli,
			const labelList& srcCellIDs,
			const boolList& mapFlag,
			const DynamicList<label>& visitedCells,
			labelList& seedCells
		) const;


	public:

		//- Run-time type information
		TypeName("cellVolumeWeight");

		// Constructors

			//- Construct from source and target meshes
		cellVolumeWeightMethod(const polyMesh& src, const polyMesh& tgt);

		//- Disallow default bitwise copy construction
		cellVolumeWeightMethod(const cellVolumeWeightMethod&) = delete;


		//- Destructor
		virtual ~cellVolumeWeightMethod();


		// Member Functions

			// Evaluate

				//- Calculate addressing and weights
		virtual void calculate
		(
			labelListList& srcToTgtAddr,
			scalarListList& srcToTgtWght,
			labelListList& tgtToTgtAddr,
			scalarListList& tgtToTgtWght
		);


		// Member Operators

			//- Disallow default bitwise assignment
		void operator=(const cellVolumeWeightMethod&) = delete;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_cellVolumeWeightMethod_Header
