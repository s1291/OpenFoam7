#pragma once
#ifndef _alphaFixedPressureFvPatchScalarField_Header
#define _alphaFixedPressureFvPatchScalarField_Header

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
	tnbLib::alphaFixedPressureFvPatchScalarField

Description
	A fixed-pressure alphaContactAngle boundary

SourceFiles
	alphaFixedPressureFvPatchScalarField.C

\*---------------------------------------------------------------------------*/

#include <fixedValueFvPatchFields.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
					 Class alphaFixedPressureFvPatch Declaration
	\*---------------------------------------------------------------------------*/

	class alphaFixedPressureFvPatchScalarField
		:
		public fixedValueFvPatchScalarField
	{
		// Private Data

			//- Fixed pressure
		scalarField p_;


	public:

		//- Runtime type information
		//TypeName("alphaFixedPressure");
		static const char* typeName_() { return "alphaFixedPressure"; }
		static FoamTransportModels_EXPORT const ::tnbLib::word typeName;
		static FoamTransportModels_EXPORT int debug;
		virtual const word& type() const { return typeName; };


		// Constructors

			//- Construct from patch and internal field
		FoamTransportModels_EXPORT alphaFixedPressureFvPatchScalarField
		(
			const fvPatch&,
			const DimensionedField<scalar, volMesh>&
		);

		//- Construct from patch, internal field and dictionary
		FoamTransportModels_EXPORT alphaFixedPressureFvPatchScalarField
		(
			const fvPatch&,
			const DimensionedField<scalar, volMesh>&,
			const dictionary&
		);

		//- Construct by mapping given alphaFixedPressureFvPatchScalarField
		//  onto a new patch
		FoamTransportModels_EXPORT alphaFixedPressureFvPatchScalarField
		(
			const alphaFixedPressureFvPatchScalarField&,
			const fvPatch&,
			const DimensionedField<scalar, volMesh>&,
			const fvPatchFieldMapper&
		);

		//- Copy constructor
		FoamTransportModels_EXPORT alphaFixedPressureFvPatchScalarField
		(
			const alphaFixedPressureFvPatchScalarField&
		);

		//- Construct and return a clone
		virtual tmp<fvPatchScalarField> clone() const
		{
			return tmp<fvPatchScalarField>
				(
					new alphaFixedPressureFvPatchScalarField(*this)
					);
		}

		//- Copy constructor setting internal field reference
		FoamTransportModels_EXPORT alphaFixedPressureFvPatchScalarField
		(
			const alphaFixedPressureFvPatchScalarField&,
			const DimensionedField<scalar, volMesh>&
		);

		//- Construct and return a clone setting internal field reference
		virtual tmp<fvPatchScalarField> clone
		(
			const DimensionedField<scalar, volMesh>& iF
		) const
		{
			return tmp<fvPatchScalarField>
				(
					new alphaFixedPressureFvPatchScalarField(*this, iF)
					);
		}


		// Member Functions

			// Access

				//- Return the alphaFixed pressure
		const scalarField& p() const
		{
			return p_;
		}

		//- Return reference to the alphaFixed pressure to allow adjustment
		scalarField& p()
		{
			return p_;
		}


		// Mapping functions

			//- Map (and resize as needed) from self given a mapping object
			//  Used to update fields following mesh topology change
		FoamTransportModels_EXPORT virtual void autoMap(const fvPatchFieldMapper&);

		//- Reverse map the given fvPatchField onto this fvPatchField
		//  Used to reconstruct fields
		FoamTransportModels_EXPORT virtual void rmap(const fvPatchScalarField&, const labelList&);


		// Evaluation functions

			//- Update the coefficients associated with the patch field
		FoamTransportModels_EXPORT virtual void updateCoeffs();


		//- Write
		FoamTransportModels_EXPORT virtual void write(Ostream&) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_alphaFixedPressureFvPatchScalarField_Header
