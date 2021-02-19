#pragma once
#ifndef _rotorDiskSource_Header
#define _rotorDiskSource_Header

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
	tnbLib::fv::rotorDiskSource

Description
	Rotor disk source

	Cell based momemtum source which approximates the mean effects of
	rotor forces on a cylindrical region within the domain.

Usage
	Example usage:
	\verbatim
		fields          (U);    // names of fields on which to apply source
		nBlades         3;      // number of blades
		tipEffect       0.96;   // normalised radius above which lift = 0

		inletFlowType   local;  // inlet flow type specification

		geometryMode    auto;   // geometry specification

		refDirection    (-1 0 0); // reference direction
								  // - used as reference for psi angle

		trimModel       fixed;  // fixed || targetForce

		flapCoeffs
		{
			beta0           0;  // coning angle [deg]
			beta1c          0;  // lateral flapping coeff (cos coeff)
			beta2s          0;  // longitudinal flapping coeff (sin coeff)
		}

		blade
		{
			// see bladeModel.H for documentation
		}

		profiles
		{
			profile1
			{
				type    lookup; // lookup || series
				...
				// see lookupProfile.H or seriesProfile.H for documentation
			}
			profile2
			{
				...
			}
		}
	\endverbatim

	Where:
	Valid options for the \c geometryMode entry include:
	- auto          : determine rototor co-ord system from cells
	- specified     : specified co-ord system

	Valid options for the \c inletFlowType entry include:
	- fixed         : specified velocity
	- local         : use local flow conditions
	- surfaceNormal : specified normal velocity (positive towards rotor)

See also
	tnbLib::bladeModel
	tnbLib::lookupProfile
	tnbLib::seriesProfile

SourceFiles
	rotorDiskSource.C
	rotorDiskSourceTemplates.C

\*---------------------------------------------------------------------------*/

#include <cellSetOption.hxx>
#include <cylindricalCS.hxx>
#include <cylindrical.hxx>
#include <NamedEnum.hxx>
#include <bladeModel.hxx>
#include <profileModelList.hxx>
#include <volFieldsFwd.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// Forward declaration of classes
	class trimModel;

	namespace fv
	{

		/*---------------------------------------------------------------------------*\
							  Class rotorDiskSource Declaration
		\*---------------------------------------------------------------------------*/

		class rotorDiskSource
			:
			public cellSetOption
		{
		public:

			enum geometryModeType
			{
				gmAuto,
				gmSpecified
			};
			static const NamedEnum<geometryModeType, 2> geometryModeTypeNames_;

			enum inletFlowType
			{
				ifFixed,
				ifSurfaceNormal,
				ifLocal
			};
			static const NamedEnum<inletFlowType, 3> inletFlowTypeNames_;


		protected:

			// Helper structures to encapsulate flap and trim data
			// Note: all input in degrees (converted to radians internally)

			struct flapData
			{
				scalar beta0;   // coning angle
				scalar beta1c;  // lateral flapping coeff (cos coeff)
				scalar beta2s;  // longitudinal flapping coeff (sin coeff)
			};


			// Protected data

				//- Reference density for incompressible case
			scalar rhoRef_;

			//- Rotational speed [rad/s]
			//  Positive anti-clockwise when looking along -ve lift direction
			scalar omega_;

			//- Number of blades
			label nBlades_;

			//- Inlet flow type
			inletFlowType inletFlow_;

			//- Inlet velocity for specified iinflow
			vector inletVelocity_;

			//- Tip effect [0-1]
			//  Ratio of blade radius beyond which lift=0
			scalar tipEffect_;

			//- Blade flap coefficients [rad/s]
			flapData flap_;

			//- Cell centre positions in local rotor frame
			//  (Cylindrical r, theta, z)
			List<point> x_;

			//- Rotation tensor for flap angle
			List<tensor> R_;

			//- Inverse rotation tensor for flap angle
			List<tensor> invR_;

			//- Area [m^2]
			List<scalar> area_;

			//- Rotor local cylindrical co-ordinate system (r, theta, z)
			cylindricalCS coordSys_;

			//- Rotor transformation co-ordinate system
			autoPtr<cylindrical> cylindrical_;

			//- Maximum radius
			scalar rMax_;

			//- Trim model
			autoPtr<trimModel> trim_;

			//- Blade data
			bladeModel blade_;

			//- Profile data
			profileModelList profiles_;


			// Protected Member Functions

				//- Check data
			FoamFvOptions_EXPORT void checkData();

			//- Set the face areas per cell, and optionally correct the rotor axis
			FoamFvOptions_EXPORT void setFaceArea(vector& axis, const bool correct);

			//- Create the co-ordinate system
			FoamFvOptions_EXPORT void createCoordinateSystem();

			//- Construct geometry
			FoamFvOptions_EXPORT void constructGeometry();

			//- Return the inlet flow field
			FoamFvOptions_EXPORT tmp<vectorField> inflowVelocity(const volVectorField& U) const;

			//- Helper function to write rotor values
			template<class Type>
			void writeField
			(
				const word& name,
				const List<Type>& values,
				const bool writeNow = false
			) const;


		public:

			//- Runtime type information
			//TypeName("rotorDisk");
			static const char* typeName_() { return "rotorDisk"; }
			static FoamFvOptions_EXPORT const ::tnbLib::word typeName;
			static FoamFvOptions_EXPORT int debug;
			virtual const word& type() const { return typeName; };


			// Constructors


				//- Construct from components
			FoamFvOptions_EXPORT rotorDiskSource
			(
				const word& name,
				const word& modelType,
				const dictionary& dict,
				const fvMesh& mesh
			);


			//- Destructor
			FoamFvOptions_EXPORT virtual ~rotorDiskSource();


			// Member Functions

				// Access

					//- Return the reference density for incompressible case
			inline scalar rhoRef() const;

			//- Return the rotational speed [rad/s]
			//  Positive anti-clockwise when looking along -ve lift direction
			inline scalar omega() const;

			//- Return the cell centre positions in local rotor frame
			// (Cylindrical r, theta, z)
			inline const List<point>& x() const;

			//- Return the rotor co-ordinate system (r, theta, z)
			inline const cylindricalCS& coordSys() const;


			// Evaluation

				//- Calculate forces
			template<class RhoFieldType>
			void calculate
			(
				const RhoFieldType& rho,
				const vectorField& U,
				const scalarField& thetag,
				vectorField& force,
				const bool divideVolume = true,
				const bool output = true
			) const;


			// Source term addition

				//- Source term to momentum equation
			FoamFvOptions_EXPORT virtual void addSup
			(
				fvMatrix<vector>& eqn,
				const label fieldi
			);

			//- Source term to compressible momentum equation
			FoamFvOptions_EXPORT virtual void addSup
			(
				const volScalarField& rho,
				fvMatrix<vector>& eqn,
				const label fieldi
			);


			// IO

				//- Read source dictionary
			FoamFvOptions_EXPORT virtual bool read(const dictionary& dict);
		};


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace fv
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <rotorDiskSourceI.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

//#ifdef NoRepository
//#include <rotorDiskSourceTemplates.cxx>
//#endif

#include <rotorDiskSourceTemplatesI.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_rotorDiskSource_Header
