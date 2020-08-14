#include <kOmega.hxx>

#include <fvOptions.hxx>
#include <bound.hxx>

#include <surfaceMesh.hxx>  // added by amir
#include <fvBoundaryMesh.hxx>  // added by amir

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace RASModels
	{

		// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

		template<class BasicTurbulenceModel>
		void kOmega<BasicTurbulenceModel>::correctNut()
		{
			this->nut_ = k_ / omega_;
			this->nut_.correctBoundaryConditions();
			fv::options::New(this->mesh_).correct(this->nut_);

			BasicTurbulenceModel::correctNut();
		}


		template<class BasicTurbulenceModel>
		tmp<fvScalarMatrix> kOmega<BasicTurbulenceModel>::kSource() const
		{
			return tmp<fvScalarMatrix>
				(
					new fvScalarMatrix
					(
						k_,
						dimVolume*this->rho_.dimensions()*k_.dimensions()
						/ dimTime
					)
					);
		}


		template<class BasicTurbulenceModel>
		tmp<fvScalarMatrix> kOmega<BasicTurbulenceModel>::omegaSource() const
		{
			return tmp<fvScalarMatrix>
				(
					new fvScalarMatrix
					(
						omega_,
						dimVolume*this->rho_.dimensions()*omega_.dimensions() / dimTime
					)
					);
		}


		// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

		template<class BasicTurbulenceModel>
		kOmega<BasicTurbulenceModel>::kOmega
		(
			const alphaField& alpha,
			const rhoField& rho,
			const volVectorField& U,
			const surfaceScalarField& alphaRhoPhi,
			const surfaceScalarField& phi,
			const transportModel& transport,
			const word& propertiesName,
			const word& type
		)
			:
			eddyViscosity<RASModel<BasicTurbulenceModel>>
			(
				type,
				alpha,
				rho,
				U,
				alphaRhoPhi,
				phi,
				transport,
				propertiesName
				),

			Cmu_
			(
				dimensioned<scalar>::lookupOrAddToDict
				(
					"betaStar",
					this->coeffDict_,
					0.09
				)
			),
			beta_
			(
				dimensioned<scalar>::lookupOrAddToDict
				(
					"beta",
					this->coeffDict_,
					0.072
				)
			),
			gamma_
			(
				dimensioned<scalar>::lookupOrAddToDict
				(
					"gamma",
					this->coeffDict_,
					0.52
				)
			),
			alphaK_
			(
				dimensioned<scalar>::lookupOrAddToDict
				(
					"alphaK",
					this->coeffDict_,
					0.5
				)
			),
			alphaOmega_
			(
				dimensioned<scalar>::lookupOrAddToDict
				(
					"alphaOmega",
					this->coeffDict_,
					0.5
				)
			),

			k_
			(
				IOobject
				(
					IOobject::groupName("k", alphaRhoPhi.group()),
					this->runTime_.timeName(),
					this->mesh_,
					IOobject::MUST_READ,
					IOobject::AUTO_WRITE
				),
				this->mesh_
			),
			omega_
			(
				IOobject
				(
					IOobject::groupName("omega", alphaRhoPhi.group()),
					this->runTime_.timeName(),
					this->mesh_,
					IOobject::MUST_READ,
					IOobject::AUTO_WRITE
				),
				this->mesh_
			)
		{
			bound(k_, this->kMin_);
			bound(omega_, this->omegaMin_);

			if (type == typeName)
			{
				this->printCoeffs(type);
			}
		}


		// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

		template<class BasicTurbulenceModel>
		bool kOmega<BasicTurbulenceModel>::read()
		{
			if (eddyViscosity<RASModel<BasicTurbulenceModel>>::read())
			{
				Cmu_.readIfPresent(this->coeffDict());
				beta_.readIfPresent(this->coeffDict());
				gamma_.readIfPresent(this->coeffDict());
				alphaK_.readIfPresent(this->coeffDict());
				alphaOmega_.readIfPresent(this->coeffDict());

				return true;
			}
			else
			{
				return false;
			}
		}


		template<class BasicTurbulenceModel>
		void kOmega<BasicTurbulenceModel>::correct()
		{
			if (!this->turbulence_)
			{
				return;
			}

			// Local references
			const alphaField& alpha = this->alpha_;
			const rhoField& rho = this->rho_;
			const surfaceScalarField& alphaRhoPhi = this->alphaRhoPhi_;
			const volVectorField& U = this->U_;
			volScalarField& nut = this->nut_;
			fv::options& fvOptions(fv::options::New(this->mesh_));

			eddyViscosity<RASModel<BasicTurbulenceModel>>::correct();

			volScalarField::Internal divU
			(
				fvc::div(fvc::absolute(this->phi(), U))().v()
			);

			tmp<volTensorField> tgradU = fvc::grad(U);
			volScalarField::Internal G
			(
				this->GName(),
				nut.v()*(dev(twoSymm(tgradU().v())) && tgradU().v())
			);
			tgradU.clear();

			// Update omega and G at the wall
			omega_.boundaryFieldRef().updateCoeffs();

			// Turbulence specific dissipation rate equation
			tmp<fvScalarMatrix> omegaEqn
			(
				fvm::ddt(alpha, rho, omega_)
				+ fvm::div(alphaRhoPhi, omega_)
				- fvm::laplacian(alpha*rho*DomegaEff(), omega_)
				==
				gamma_ * alpha()*rho()*G*omega_() / k_()
				- fvm::SuSp(((2.0 / 3.0)*gamma_)*alpha()*rho()*divU, omega_)
				- fvm::Sp(beta_*alpha()*rho()*omega_(), omega_)
				+ omegaSource()
				+ fvOptions(alpha, rho, omega_)
			);

			omegaEqn.ref().relax();
			fvOptions.constrain(omegaEqn.ref());
			omegaEqn.ref().boundaryManipulate(omega_.boundaryFieldRef());
			solve(omegaEqn);
			fvOptions.correct(omega_);
			bound(omega_, this->omegaMin_);


			// Turbulent kinetic energy equation
			tmp<fvScalarMatrix> kEqn
			(
				fvm::ddt(alpha, rho, k_)
				+ fvm::div(alphaRhoPhi, k_)
				- fvm::laplacian(alpha*rho*DkEff(), k_)
				==
				alpha()*rho()*G
				- fvm::SuSp((2.0 / 3.0)*alpha()*rho()*divU, k_)
				- fvm::Sp(Cmu_*alpha()*rho()*omega_(), k_)
				+ kSource()
				+ fvOptions(alpha, rho, k_)
			);

			kEqn.ref().relax();
			fvOptions.constrain(kEqn.ref());
			solve(kEqn);
			fvOptions.correct(k_);
			bound(k_, this->kMin_);

			correctNut();
		}


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace RASModels
} // End namespace tnbLib

// ************************************************************************* //