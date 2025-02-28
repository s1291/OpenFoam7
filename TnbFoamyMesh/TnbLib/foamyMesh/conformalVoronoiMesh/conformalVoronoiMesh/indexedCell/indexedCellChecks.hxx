#pragma once
#ifndef _indexedCellChecks_Header
#define _indexedCellChecks_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2012-2018 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include <FoamyMesh_Module.hxx>
#include <scalar.hxx>

namespace tnbLib
{

	namespace foamyHexMeshChecks
	{

		template<class Cell>
		scalar coplanarTet
		(
			Cell& c,
			const scalar tol = 1e-12
		);

		template<class Cell>
		bool closePoints
		(
			Cell& c,
			const scalar tol = 1e-12
		);

		template<class Cell>
		bool smallVolume
		(
			Cell& c,
			const scalar tol = 0.0
		);


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace foamyHexMeshChecks

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_indexedCellChecks_Header
