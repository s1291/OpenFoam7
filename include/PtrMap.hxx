#pragma once
#ifndef _PtrMap_Header
#define _PtrMap_Header

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
	tnbLib::PtrMap

Description
	A HashTable of pointers to objects of type \<T\> with a label key.

See also
	Map, HashPtrTable

\*---------------------------------------------------------------------------*/

#include <HashPtrTable.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							   Class PtrMap Declaration
	\*---------------------------------------------------------------------------*/

	template<class T>
	class PtrMap
		:
		public HashPtrTable<T, label, Hash<label>>
	{

	public:

		// Constructors

			//- Construct given initial map size
		PtrMap(const label size = 128)
			:
			HashPtrTable<T, label, Hash<label>>(size)
		{}

		//- Construct from Istream
		PtrMap(Istream& is)
			:
			HashPtrTable<T, label, Hash<label>>(is)
		{}

		//- Copy construct
		PtrMap(const PtrMap<T>& map)
			:
			HashPtrTable<T, label, Hash<label>>(map)
		{}

		//- Move constructor
		PtrMap(PtrMap<T>&& map)
			:
			HashPtrTable<T, label, Hash<label>>(move(map))
		{}


		// Member Operators

		void operator=(const PtrMap<T>& map)
		{
			HashPtrTable<T, label, Hash<label>>::operator=(map);
		}

		void operator=(PtrMap<T>&& map)
		{
			HashPtrTable<T, label, Hash<label>>::operator=(move(map));
		}
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_PtrMap_Header
