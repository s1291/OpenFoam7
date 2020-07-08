#pragma once
#ifndef _tableReader_Header
#define _tableReader_Header

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
	tnbLib::tableReader

Description
	Base class to read table data for the interpolationTable

SourceFiles
	tableReader.C

\*---------------------------------------------------------------------------*/

#include <fileName.hxx>
#include <wordList.hxx>
#include <vector.hxx>
#include <tensor.hxx>
#include <typeInfo.hxx>
#include <runTimeSelectionTables.hxx>
#include <autoPtr.hxx>
#include <dictionary.hxx>
#include <Tuple2.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							 Class tableReader Declaration
	\*---------------------------------------------------------------------------*/

	template<class Type>
	class tableReader
	{

	public:

		//- Runtime type information
		TypeName("tableReader");

		// Declare run-time constructor selection table

		declareRunTimeSelectionTable
		(
			autoPtr,
			tableReader,
			dictionary,
			(const dictionary& dict),
			(dict)
		);


		// Constructors

			//- Construct from dictionary
		tableReader(const dictionary& dict);

		//- Construct and return a clone
		virtual autoPtr<tableReader<Type>> clone() const = 0;


		// Selectors

			//- Return a reference to the selected tableReader
		static autoPtr<tableReader> New(const dictionary& spec);


		//- Destructor
		virtual ~tableReader();


		// Member Functions

			//- Read the table
		virtual void operator()
			(
				const fileName&,
				List<Tuple2<scalar, Type>>&
				) = 0;

		//- Read the 2D table
		virtual void operator()
			(
				const fileName&,
				List<Tuple2<scalar, List<Tuple2<scalar, Type>>>>&
				) = 0;

		//- Write additional information
		virtual void write(Ostream& os) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#include <tableReader.cxx>
#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_tableReader_Header
