#pragma once
#ifndef _zeroFieldField_Header
#define _zeroFieldField_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
	\\  /    A nd           | Copyright (C) 2011-2018 OpenFOAM Foundation
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
	tnbLib::zeroFieldField

Description
	A class representing the concept of a field of zeroFields used to
	avoid unnecessary manipulations for objects which are known to be zero at
	compile-time.

	Used for example as the density argument to a function written for
	compressible to be used for incompressible flow.

\*---------------------------------------------------------------------------*/

#include <zeroField.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							   Class zeroField Declaration
	\*---------------------------------------------------------------------------*/

	class zeroFieldField
		:
		public zero
	{

	public:

		// Constructors

			//- Construct null
		zeroFieldField()
		{}


		// Member Operators

		inline zeroField operator[](const label) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <zeroFieldFieldI.hxx>

#endif // !_zeroFieldField_Header
