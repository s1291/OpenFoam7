#pragma once
#ifndef _nutkRoughWallFunctionFvPatchScalarField_Header
#define _nutkRoughWallFunctionFvPatchScalarField_Header

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
	tnbLib::nutkRoughWallFunctionFvPatchScalarField

Description
	This boundary condition provides a turbulent kinematic viscosity condition
	when using wall functions for rough walls, based on turbulence kinetic
	energy.  The condition manipulates the E parameter to account for roughness
	effects.

Usage
	\table
		Property     | Description             | Required    | Default value
		Ks           | sand-grain roughness height | yes     |
		Cs           | roughness constant      | yes         |
	\endtable

	Parameter ranges
	- roughness height (Ks) = sand-grain roughness (0 for smooth walls)
	- roughness constant (Cs) = 0.5 - 1.0

	Example of the boundary condition specification:
	\verbatim
	<patchName>
	{
		type            nutkRoughWallFunction;
		Ks              uniform 1e-5;
		Cs              uniform 0.5;
		value           uniform 0;
	}
	\endverbatim

See also
	tnbLib::nutkWallFunctionFvPatchScalarField

SourceFiles
	nutkRoughWallFunctionFvPatchScalarField.C

\*---------------------------------------------------------------------------*/

#include <nutkWallFunctionFvPatchScalarField.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
			   Class nutkRoughWallFunctionFvPatchScalarField Declaration
	\*---------------------------------------------------------------------------*/

	class nutkRoughWallFunctionFvPatchScalarField
		:
		public nutkWallFunctionFvPatchScalarField
	{
		// Private Data

			//- Roughness height
		scalarField Ks_;

		//- Roughness constant
		scalarField Cs_;


		// Private Member Functions

			//- Return the roughness dependent E
		scalar E(const scalar KsPlus, const scalar Cs) const;


	protected:

		// Protected Member Functions

			//- Calculate the turbulence viscosity
		virtual tmp<scalarField> nut() const;


	public:

		//- Runtime type information
		TypeName("nutkRoughWallFunction");


		// Constructors

			//- Construct from patch and internal field
		nutkRoughWallFunctionFvPatchScalarField
		(
			const fvPatch&,
			const DimensionedField<scalar, volMesh>&
		);

		//- Construct from patch, internal field and dictionary
		nutkRoughWallFunctionFvPatchScalarField
		(
			const fvPatch&,
			const DimensionedField<scalar, volMesh>&,
			const dictionary&
		);

		//- Construct by mapping given
		//  nutkRoughWallFunctionFvPatchScalarField
		//  onto a new patch
		nutkRoughWallFunctionFvPatchScalarField
		(
			const nutkRoughWallFunctionFvPatchScalarField&,
			const fvPatch&,
			const DimensionedField<scalar, volMesh>&,
			const fvPatchFieldMapper&
		);

		//- Copy constructor
		nutkRoughWallFunctionFvPatchScalarField
		(
			const nutkRoughWallFunctionFvPatchScalarField&
		);

		//- Construct and return a clone
		virtual tmp<fvPatchScalarField> clone() const
		{
			return tmp<fvPatchScalarField>
				(
					new nutkRoughWallFunctionFvPatchScalarField(*this)
					);
		}

		//- Copy constructor setting internal field reference
		nutkRoughWallFunctionFvPatchScalarField
		(
			const nutkRoughWallFunctionFvPatchScalarField&,
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
					new nutkRoughWallFunctionFvPatchScalarField(*this, iF)
					);
		}


		// Member Functions

			// Access functions

				//- Return the roughness height
		const scalarField& Ks() const
		{
			return Ks_;
		}

		//- Return reference to the roughness height to allow adjustment
		scalarField& Ks()
		{
			return Ks_;
		}


		//- Return the roughness constant scale
		const scalarField& Cs() const
		{
			return Cs_;
		}

		//- Return reference to the roughness constant to allow adjustment
		scalarField& Cs()
		{
			return Cs_;
		}


		// Mapping functions

			//- Map (and resize as needed) from self given a mapping object
		virtual void autoMap(const fvPatchFieldMapper&);

		//- Reverse map the given fvPatchField onto this fvPatchField
		virtual void rmap
		(
			const fvPatchScalarField&,
			const labelList&
		);


		// I-O

			//- Write
		virtual void write(Ostream&) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_nutkRoughWallFunctionFvPatchScalarField_Header
