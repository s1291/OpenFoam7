#pragma once
#ifndef _fixedNormalSlipPointPatchField_Header
#define _fixedNormalSlipPointPatchField_Header

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
	tnbLib::fixedNormalSlipPointPatchField

Description
	slip with user-specified normal

SourceFiles
	fixedNormalSlipPointPatchField.C

\*---------------------------------------------------------------------------*/

#include <slipPointPatchField.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// Forward declaration of friend functions and operators

	template<class Type>
	class fixedNormalSlipPointPatchField;

	template<class Type>
	Ostream& operator<<
		(
			Ostream&,
			const fixedNormalSlipPointPatchField<Type>&
			);


	/*---------------------------------------------------------------------------*\
							Class fixedNormalSlipPointPatchField Declaration
	\*---------------------------------------------------------------------------*/

	template<class Type>
	class fixedNormalSlipPointPatchField
		:
		public slipPointPatchField<Type>
	{
		// Private Data

			//- User specified normal
		vector n_;


	public:

		//- Runtime type information
		TypeName("fixedNormalSlip");


		// Constructors

			//- Construct from patch and internal field
		fixedNormalSlipPointPatchField
		(
			const pointPatch&,
			const DimensionedField<Type, pointMesh>&
		);

		//- Construct from patch, internal field and dictionary
		fixedNormalSlipPointPatchField
		(
			const pointPatch&,
			const DimensionedField<Type, pointMesh>&,
			const dictionary&
		);

		//- Construct by mapping given patchField<Type> onto a new patch
		fixedNormalSlipPointPatchField
		(
			const fixedNormalSlipPointPatchField<Type>&,
			const pointPatch&,
			const DimensionedField<Type, pointMesh>&,
			const pointPatchFieldMapper&
		);

		//- Construct and return a clone
		virtual autoPtr<pointPatchField<Type>> clone() const
		{
			return autoPtr<pointPatchField<Type>>
				(
					new fixedNormalSlipPointPatchField<Type>
					(
						*this
						)
					);
		}

		//- Construct as copy setting internal field reference
		fixedNormalSlipPointPatchField
		(
			const fixedNormalSlipPointPatchField<Type>&,
			const DimensionedField<Type, pointMesh>&
		);

		//- Construct and return a clone setting internal field reference
		virtual autoPtr<pointPatchField<Type>> clone
		(
			const DimensionedField<Type, pointMesh>& iF
		) const
		{
			return autoPtr<pointPatchField<Type>>
				(
					new fixedNormalSlipPointPatchField<Type>
					(
						*this,
						iF
						)
					);
		}

		// Member Functions

			//- Update the patch field
		virtual void evaluate
		(
			const Pstream::commsTypes commsType = Pstream::commsTypes::blocking
		);

		//- Write
		virtual void write(Ostream&) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <fixedNormalSlipPointPatchFieldI.hxx>

//#ifdef NoRepository
//#include <fixedNormalSlipPointPatchField.cxx>
//#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_fixedNormalSlipPointPatchField_Header
