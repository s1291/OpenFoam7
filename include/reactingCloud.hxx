#pragma once
#ifndef _reactingCloud_Header
#define _reactingCloud_Header

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
	tnbLib::reactingCloud

Description
	Virtual abstract base class for templated ReactingCloud

SourceFiles
	reactingCloud.C

\*---------------------------------------------------------------------------*/

#include <typeInfo.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
						   Class reactingCloud Declaration
	\*---------------------------------------------------------------------------*/

	class reactingCloud
	{
	public:

		//- Runtime type information
		TypeName("reactingCloud");

		// Constructors

			//- Null constructor
		reactingCloud();

		//- Disallow default bitwise copy construction
		reactingCloud(const reactingCloud&) = delete;


		//- Destructor
		virtual ~reactingCloud();


		// Member Operators

			//- Disallow default bitwise assignment
		void operator=(const reactingCloud&) = delete;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_reactingCloud_Header
