#pragma once
#ifndef _DiagonalPreconditionerTemplate_Header
#define _DiagonalPreconditionerTemplate_Header

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
	tnbLib::DiagonalPreconditioner

Description
	Diagonal preconditioner for both symmetric and asymmetric matrices.

	The inverse (reciprocal for scalar) of the diagonal is calculated and
	stored.

SourceFiles
	DiagonalPreconditioner.C

\*---------------------------------------------------------------------------*/

#include <LduMatrixTemplate.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							   Class DiagonalPreconditioner Declaration
	\*---------------------------------------------------------------------------*/

	template<class Type, class DType, class LUType>
	class DiagonalPreconditioner
		:
		public LduMatrix<Type, DType, LUType>::preconditioner
	{
		// Private Data

			//- The inverse (reciprocal for scalar) diagonal
		Field<DType> rD;


	public:

		//- Runtime type information
		TypeName("diagonal");


		// Constructors

			//- Construct from matrix components and preconditioner data dictionary
		DiagonalPreconditioner
		(
			const typename LduMatrix<Type, DType, LUType>::solver& sol,
			const dictionary& preconditionerDict
		);

		//- Disallow default bitwise copy construction
		DiagonalPreconditioner(const DiagonalPreconditioner&) = delete;


		// Destructor

		virtual ~DiagonalPreconditioner()
		{}


		// Member Functions

			//- Read and reset the preconditioner parameters from the given
			//  dictionary
		virtual void read(const dictionary& preconditionerDict);

		//- Return wA the preconditioned form of residual rA
		virtual void precondition
		(
			Field<Type>& wA,
			const Field<Type>& rA
		) const;

		//- Return wT the transpose-matrix preconditioned form of
		//  residual rT.
		virtual void preconditionT
		(
			Field<Type>& wT,
			const Field<Type>& rT
		) const
		{
			return(precondition(wT, rT));
		}


		// Member Operators

			//- Disallow default bitwise assignment
		void operator=(const DiagonalPreconditioner&) = delete;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <DiagonalPreconditionerTemplateI.hxx>

//#ifdef NoRepository
//#include <DiagonalPreconditionerTemplate.cxx>
//#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_DiagonalPreconditionerTemplate_Header
