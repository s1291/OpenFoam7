#pragma once
#ifndef _wedgePointPatchField_Header
#define _wedgePointPatchField_Header

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
	tnbLib::wedgePointPatchField

Description
	Wedge front and back plane patch field

SourceFiles
	wedgePointPatchField.C

\*---------------------------------------------------------------------------*/

#include <pointPatchField.hxx>
#include <wedgePointPatch.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
						Class wedgePointPatchField Declaration
	\*---------------------------------------------------------------------------*/

	template<class Type>
	class wedgePointPatchField
		:
		public pointPatchField<Type>
	{

	public:

		//- Runtime type information
		TypeName(wedgePointPatch::typeName_());


		// Constructors

			//- Construct from patch and internal field
		wedgePointPatchField
		(
			const pointPatch&,
			const DimensionedField<Type, pointMesh>&
		);

		//- Construct from patch, internal field and dictionary
		wedgePointPatchField
		(
			const pointPatch&,
			const DimensionedField<Type, pointMesh>&,
			const dictionary&
		);

		//- Construct by mapping given patchField<Type> onto a new patch
		wedgePointPatchField
		(
			const wedgePointPatchField<Type>&,
			const pointPatch&,
			const DimensionedField<Type, pointMesh>&,
			const pointPatchFieldMapper&
		);

		//- Construct and return a clone
		virtual autoPtr<pointPatchField<Type>> clone() const
		{
			return autoPtr<pointPatchField<Type>>
				(
					new wedgePointPatchField<Type>
					(
						*this
						)
					);
		}

		//- Construct as copy setting internal field reference
		wedgePointPatchField
		(
			const wedgePointPatchField<Type>&,
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
					new wedgePointPatchField<Type>
					(
						*this, iF
						)
					);
		}


		// Member Functions

			//- Return the constraint type this pointPatchField implements
		virtual const word& constraintType() const
		{
			return type();
		}

		//- Update the patch field
		virtual void evaluate
		(
			const Pstream::commsTypes commsType = Pstream::commsTypes::blocking
		);
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <wedgePointPatchFieldI.hxx>

//#ifdef NoRepository
//#include <wedgePointPatchField.cxx>
//#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_wedgePointPatchField_Header
