#pragma once
#ifndef _LESModel_Header
#define _LESModel_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
	\\  /    A nd           | Copyright (C) 2013-2019 OpenFOAM Foundation
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

Namespace
	tnbLib::LESModels

Description
	Namespace for LES SGS models.

Class
	tnbLib::LESModel

Description
	Templated abstract base class for LES SGS models

SourceFiles
	LESModel.C

\*---------------------------------------------------------------------------*/

#include <TurbulenceModelTemplate.hxx>
#include <LESdelta.hxx>

#include <dictionary.hxx>  // added by amir
#include <Switch.hxx>  // added by amir
#include <dimensionedScalar.hxx>  // added by amir
#include <autoPtr.hxx>  // added by amir
#include <LESdelta.hxx>  // added by amir

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							   Class LESModel Declaration
	\*---------------------------------------------------------------------------*/

	template<class BasicTurbulenceModel>
	class LESModel
		:
		public BasicTurbulenceModel
	{

	protected:

		// Protected data

			//- LES coefficients dictionary
		dictionary LESDict_;

		//- Turbulence on/off flag
		Switch turbulence_;

		//- Flag to print the model coeffs at run-time
		Switch printCoeffs_;

		//- Model coefficients dictionary
		dictionary coeffDict_;

		//- Lower limit of k
		dimensionedScalar kMin_;

		//- Lower limit of epsilon
		dimensionedScalar epsilonMin_;

		//- Lower limit for omega
		dimensionedScalar omegaMin_;

		//- Run-time selectable delta model
		autoPtr<tnbLib::LESdelta> delta_;


		// Protected Member Functions

			//- Print model coefficients
		virtual void printCoeffs(const word& type);


	public:

		typedef typename BasicTurbulenceModel::alphaField alphaField;
		typedef typename BasicTurbulenceModel::rhoField rhoField;
		typedef typename BasicTurbulenceModel::transportModel transportModel;


		//- Runtime type information
		TypeName("LES");


		// Declare run-time constructor selection table

		declareRunTimeSelectionTable
		(
			autoPtr,
			LESModel,
			dictionary,
			(
				const alphaField& alpha,
				const rhoField& rho,
				const volVectorField& U,
				const surfaceScalarField& alphaRhoPhi,
				const surfaceScalarField& phi,
				const transportModel& transport,
				const word& propertiesName
				),
				(alpha, rho, U, alphaRhoPhi, phi, transport, propertiesName)
		);


		// Constructors

			//- Construct from components
		LESModel
		(
			const word& type,
			const alphaField& alpha,
			const rhoField& rho,
			const volVectorField& U,
			const surfaceScalarField& alphaRhoPhi,
			const surfaceScalarField& phi,
			const transportModel& transport,
			const word& propertiesName
		);

		//- Disallow default bitwise copy construction
		LESModel(const LESModel&) = delete;


		// Selectors

			//- Return a reference to the selected LES model
		static autoPtr<LESModel> New
		(
			const alphaField& alpha,
			const rhoField& rho,
			const volVectorField& U,
			const surfaceScalarField& alphaRhoPhi,
			const surfaceScalarField& phi,
			const transportModel& transport,
			const word& propertiesName = turbulenceModel::propertiesName
		);


		//- Destructor
		virtual ~LESModel()
		{}


		// Member Functions

			//- Read model coefficients if they have changed
		virtual bool read();


		// Access

			//- Const access to the coefficients dictionary
		virtual const dictionary& coeffDict() const
		{
			return coeffDict_;
		}

		//- Return the lower allowable limit for k (default: small)
		const dimensionedScalar& kMin() const
		{
			return kMin_;
		}

		//- Allow kMin to be changed
		dimensionedScalar& kMin()
		{
			return kMin_;
		}

		//- Access function to filter width
		inline const volScalarField& delta() const
		{
			return delta_();
		}


		//- Return the effective viscosity
		virtual tmp<volScalarField> nuEff() const
		{
			return volScalarField::New
			(
				IOobject::groupName("nuEff", this->alphaRhoPhi_.group()),
				this->nut() + this->nu()
			);
		}

		//- Return the effective viscosity on patch
		virtual tmp<scalarField> nuEff(const label patchi) const
		{
			return this->nut(patchi) + this->nu(patchi);
		}

		//- Solve the turbulence equations and correct the turbulence viscosity
		virtual void correct();


		// Member Operators

			//- Disallow default bitwise assignment
		void operator=(const LESModel&) = delete;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <LESModelI.hxx>

//#ifdef NoRepository
//#include <LESModel.cxx>
//#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_LESModel_Header
