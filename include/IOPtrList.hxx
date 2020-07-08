#pragma once
#ifndef _IOPtrList_Header
#define _IOPtrList_Header

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
	tnbLib::IOPtrList

Description
	A PtrList of objects of type \<T\> with automated input and output.

SourceFiles
	IOPtrList.C

\*---------------------------------------------------------------------------*/

#include <PtrList.hxx>
#include <regIOobject.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							   Class IOPtrList Declaration
	\*---------------------------------------------------------------------------*/

	template<class T>
	class IOPtrList
		:
		public regIOobject,
		public PtrList<T>
	{

	public:

		//- Runtime type information
		TypeName("PtrList");


		// Constructors

			//- Construct from IOobject using given Istream constructor class
		template<class INew>
		IOPtrList(const IOobject&, const INew&);

		//- Construct from IOobject
		IOPtrList(const IOobject&);

		//- Construct from IOobject with given size
		IOPtrList(const IOobject&, const label);

		//- Construct from IOobject and a PtrList
		IOPtrList(const IOobject&, const PtrList<T>&);

		//- Move construct by transferring the PtrList contents
		IOPtrList(const IOobject&, PtrList<T>&&);

		//- Move constructor
		IOPtrList(IOPtrList<T>&&);


		//- Destructor
		virtual ~IOPtrList();


		// Member Functions

		bool writeData(Ostream&) const;


		// Member Operators

		void operator=(const IOPtrList<T>&);
		void operator=(IOPtrList<T>&&);
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#include <IOPtrList.cxx>
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_IOPtrList_Header
