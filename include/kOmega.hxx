#pragma once
#ifndef _kOmega_Header
#define _kOmega_Header

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
	tnbLib::RASModels::kOmega

Description
	Standard high Reynolds-number k-omega turbulence model for
	incompressible and compressible flows.

	References:
	\verbatim
		Wilcox, D. C. (1998).
		Turbulence modeling for CFD
		(Vol. 2, pp. 103-217). La Canada, CA: DCW industries.
	\endverbatim

	The default model coefficients are
	\verbatim
		kOmegaCoeffs
		{
			Cmu         0.09;  // Equivalent to betaStar
			alpha       0.52;
			beta        0.072;
			alphak      0.5;
			alphaOmega  0.5;
		}
	\endverbatim

SourceFiles
	kOmega.C

\*---------------------------------------------------------------------------*/

#include <RASModel.hxx>
#include <eddyViscosity.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace RASModels
	{

		/*---------------------------------------------------------------------------*\
								   Class kOmega Declaration
		\*---------------------------------------------------------------------------*/

		template<class BasicTurbulenceModel>
		class kOmega
			:
			public eddyViscosity<RASModel<BasicTurbulenceModel>>
		{

		protected:

			// Protected data

				// Model coefficients

			dimensionedScalar Cmu_;
			dimensionedScalar beta_;
			dimensionedScalar gamma_;
			dimensionedScalar alphaK_;
			dimensionedScalar alphaOmega_;


			// Fields

			volScalarField k_;
			volScalarField omega_;


			// Protected Member Functions

			virtual void correctNut();
			virtual tmp<fvScalarMatrix> kSource() const;
			virtual tmp<fvScalarMatrix> omegaSource() const;


		public:

			typedef typename BasicTurbulenceModel::alphaField alphaField;
			typedef typename BasicTurbulenceModel::rhoField rhoField;
			typedef typename BasicTurbulenceModel::transportModel transportModel;


			//- Runtime type information
			TypeName("kOmega");


			// Constructors

				//- Construct from components
			kOmega
			(
				const alphaField& alpha,
				const rhoField& rho,
				const volVectorField& U,
				const surfaceScalarField& alphaRhoPhi,
				const surfaceScalarField& phi,
				const transportModel& transport,
				const word& propertiesName = turbulenceModel::propertiesName,
				const word& type = typeName
			);


			//- Destructor
			virtual ~kOmega()
			{}


			// Member Functions

				//- Read RASProperties dictionary
			virtual bool read();

			//- Return the effective diffusivity for k
			tmp<volScalarField> DkEff() const
			{
				return volScalarField::New
				(
					"DkEff",
					alphaK_*this->nut_ + this->nu()
				);
			}

			//- Return the effective diffusivity for omega
			tmp<volScalarField> DomegaEff() const
			{
				return volScalarField::New
				(
					"DomegaEff",
					alphaOmega_*this->nut_ + this->nu()
				);
			}

			//- Return the turbulence kinetic energy
			virtual tmp<volScalarField> k() const
			{
				return k_;
			}

			//- Return the turbulence specific dissipation rate
			virtual tmp<volScalarField> omega() const
			{
				return omega_;
			}

			//- Return the turbulence kinetic energy dissipation rate
			virtual tmp<volScalarField> epsilon() const
			{
				return volScalarField::New
				(
					"epsilon",
					Cmu_*k_*omega_,
					omega_.boundaryField().types()
				);
			}

			//- Solve the turbulence equations and correct the turbulence viscosity
			virtual void correct();
		};


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace RASModels
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <kOmegaI.hxx>

//#ifdef NoRepository
//#include <kOmega.cxx>
//#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_kOmega_Header
