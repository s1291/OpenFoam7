#pragma once
#ifndef _singleLayerRegion_Header
#define _singleLayerRegion_Header

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
	tnbLib::regionModels::singleLayerRegion

Description
	Base class for single layer region models

SourceFiles
	singleLayerRegion.C

\*---------------------------------------------------------------------------*/

#include <regionModel.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace regionModels
	{

		/*---------------------------------------------------------------------------*\
							  Class singleLayerRegion Declaration
		\*---------------------------------------------------------------------------*/

		class singleLayerRegion
			:
			public regionModel
		{
			// Private Member Functions

				//- Construct region mesh and fields
			void constructMeshObjects();

			//- Initialise the region
			void initialise();


		protected:

			// Protected data

				// Region addressing

					//- Patch normal vectors
			autoPtr<volVectorField> nHatPtr_;

			//- Face area magnitudes / [m^2]
			autoPtr<volScalarField> magSfPtr_;


			// Addressing

				//- List of patch IDs opposite to internally coupled patches
			labelList passivePatchIDs_;


			// Protected member functions

				//- Read control parameters from dictionary
			virtual bool read();


		public:

			//- Runtime type information
			TypeName("regionModel");


			// Constructors

				//- Construct null
			singleLayerRegion(const fvMesh& mesh, const word& regionType);

			//- Construct from mesh, region type and name
			singleLayerRegion
			(
				const fvMesh& mesh,
				const word& regionType,
				const word& modelName,
				bool readFields = true
			);

			//- Disallow default bitwise copy construction
			singleLayerRegion(const singleLayerRegion&) = delete;


			//- Destructor
			virtual ~singleLayerRegion();


			// Member Functions

				// Access

					// Region geometry

						//- Return the patch normal vectors
			virtual const volVectorField& nHat() const;

			//- Return the face area magnitudes / [m^2]
			virtual const volScalarField& magSf() const;


			// Addressing

				//- Return the list of patch IDs opposite to internally
				//  coupled patches
			virtual const labelList& passivePatchIDs() const;


			// Patch type information

				//- Return boundary types for mapped field patches
				//  Also maps internal field value
				//  Mapping region prescribed by underlying mapped poly patch
			template<class Type>
			wordList mappedFieldAndInternalPatchTypes() const;

			//- Return boundary types for pushed mapped field patches
			//  Mapping region prescribed by underlying mapped poly patch
			template<class Type>
			wordList mappedPushedFieldPatchTypes() const;


			// Member Operators

				//- Disallow default bitwise assignment
			void operator=(const singleLayerRegion&) = delete;
		};


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace regionModels
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#include <singleLayerRegionTemplates.cxx>
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_singleLayerRegion_Header
