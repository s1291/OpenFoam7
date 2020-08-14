#include <kOmegaSSTBase.hxx>

#include <fvOptions.hxx>
#include <bound.hxx>
#include <wallDist.hxx>

#include <fvBoundaryMesh.hxx>  // added by amir
#include <surfaceFields.hxx>  // added by amir

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * * //

	template<class TurbulenceModel, class BasicTurbulenceModel>
	tmp<volScalarField>
		kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::kOmegaSST::F1
		(
			const volScalarField& CDkOmega
		) const
	{
		tmp<volScalarField> CDkOmegaPlus = max
		(
			CDkOmega,
			dimensionedScalar(dimless / sqr(dimTime), 1.0e-10)
		);

		tmp<volScalarField> arg1 = min
		(
			min
			(
				max
				(
				(scalar(1) / betaStar_)*sqrt(k_) / (omega_*y_),
					scalar(500)*(this->mu() / this->rho_) / (sqr(y_)*omega_)
				),
				(4 * alphaOmega2_)*k_ / (CDkOmegaPlus*sqr(y_))
			),
			scalar(10)
		);

		return tanh(pow4(arg1));
	}

	template<class TurbulenceModel, class BasicTurbulenceModel>
	tmp<volScalarField>
		kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::kOmegaSST::F2() const
	{
		tmp<volScalarField> arg2 = min
		(
			max
			(
			(scalar(2) / betaStar_)*sqrt(k_) / (omega_*y_),
				scalar(500)*(this->mu() / this->rho_) / (sqr(y_)*omega_)
			),
			scalar(100)
		);

		return tanh(sqr(arg2));
	}

	template<class TurbulenceModel, class BasicTurbulenceModel>
	tmp<volScalarField>
		kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::kOmegaSST::F3() const
	{
		tmp<volScalarField> arg3 = min
		(
			150 * (this->mu() / this->rho_) / (omega_*sqr(y_)),
			scalar(10)
		);

		return 1 - tanh(pow4(arg3));
	}

	template<class TurbulenceModel, class BasicTurbulenceModel>
	tmp<volScalarField>
		kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::kOmegaSST::F23() const
	{
		tmp<volScalarField> f23(F2());

		if (F3_)
		{
			f23.ref() *= F3();
		}

		return f23;
	}


	template<class TurbulenceModel, class BasicTurbulenceModel>
	void kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::correctNut
	(
		const volScalarField& S2,
		const volScalarField& F2
	)
	{
		this->nut_ = a1_ * k_ / max(a1_*omega_, b1_*F2*sqrt(S2));
		this->nut_.correctBoundaryConditions();
		fv::options::New(this->mesh_).correct(this->nut_);

		BasicTurbulenceModel::correctNut();
	}


	// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

	template<class TurbulenceModel, class BasicTurbulenceModel>
	void kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::correctNut()
	{
		correctNut(2 * magSqr(symm(fvc::grad(this->U_))), F23());
	}


	template<class TurbulenceModel, class BasicTurbulenceModel>
	tmp<volScalarField::Internal>
		kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::Pk
		(
			const volScalarField::Internal& G
		) const
	{
		return min(G, (c1_*betaStar_)*this->k_()*this->omega_());
	}


	template<class TurbulenceModel, class BasicTurbulenceModel>
	tmp<volScalarField::Internal>
		kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::epsilonByk
		(
			const volScalarField::Internal& F1,
			const volScalarField::Internal& F2
		) const
	{
		return betaStar_ * omega_();
	}


	template<class TurbulenceModel, class BasicTurbulenceModel>
	tmp<fvScalarMatrix>
		kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::kSource() const
	{
		return tmp<fvScalarMatrix>
			(
				new fvScalarMatrix
				(
					k_,
					dimVolume*this->rho_.dimensions()*k_.dimensions() / dimTime
				)
				);
	}


	template<class TurbulenceModel, class BasicTurbulenceModel>
	tmp<fvScalarMatrix>
		kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::omegaSource() const
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


	template<class TurbulenceModel, class BasicTurbulenceModel>
	tmp<fvScalarMatrix> kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::Qsas
	(
		const volScalarField::Internal& S2,
		const volScalarField::Internal& gamma,
		const volScalarField::Internal& beta
	) const
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

	template<class TurbulenceModel, class BasicTurbulenceModel>
	kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::kOmegaSST
	(
		const word& type,
		const alphaField& alpha,
		const rhoField& rho,
		const volVectorField& U,
		const surfaceScalarField& alphaRhoPhi,
		const surfaceScalarField& phi,
		const transportModel& transport,
		const word& propertiesName
	)
		:
		TurbulenceModel
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

		alphaK1_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"alphaK1",
				this->coeffDict_,
				0.85
			)
		),
		alphaK2_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"alphaK2",
				this->coeffDict_,
				1.0
			)
		),
		alphaOmega1_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"alphaOmega1",
				this->coeffDict_,
				0.5
			)
		),
		alphaOmega2_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"alphaOmega2",
				this->coeffDict_,
				0.856
			)
		),
		gamma1_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"gamma1",
				this->coeffDict_,
				5.0 / 9.0
			)
		),
		gamma2_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"gamma2",
				this->coeffDict_,
				0.44
			)
		),
		beta1_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"beta1",
				this->coeffDict_,
				0.075
			)
		),
		beta2_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"beta2",
				this->coeffDict_,
				0.0828
			)
		),
		betaStar_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"betaStar",
				this->coeffDict_,
				0.09
			)
		),
		a1_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"a1",
				this->coeffDict_,
				0.31
			)
		),
		b1_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"b1",
				this->coeffDict_,
				1.0
			)
		),
		c1_
		(
			dimensioned<scalar>::lookupOrAddToDict
			(
				"c1",
				this->coeffDict_,
				10.0
			)
		),
		F3_
		(
			Switch::lookupOrAddToDict
			(
				"F3",
				this->coeffDict_,
				false
			)
		),

		y_(wallDist::New(this->mesh_).y()),

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
	}


	// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

	template<class TurbulenceModel, class BasicTurbulenceModel>
	bool kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::read()
	{
		if (TurbulenceModel::read())
		{
			alphaK1_.readIfPresent(this->coeffDict());
			alphaK2_.readIfPresent(this->coeffDict());
			alphaOmega1_.readIfPresent(this->coeffDict());
			alphaOmega2_.readIfPresent(this->coeffDict());
			gamma1_.readIfPresent(this->coeffDict());
			gamma2_.readIfPresent(this->coeffDict());
			beta1_.readIfPresent(this->coeffDict());
			beta2_.readIfPresent(this->coeffDict());
			betaStar_.readIfPresent(this->coeffDict());
			a1_.readIfPresent(this->coeffDict());
			b1_.readIfPresent(this->coeffDict());
			c1_.readIfPresent(this->coeffDict());
			F3_.readIfPresent("F3", this->coeffDict());

			return true;
		}
		else
		{
			return false;
		}
	}


	template<class TurbulenceModel, class BasicTurbulenceModel>
	void kOmegaSST<TurbulenceModel, BasicTurbulenceModel>::correct()
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

		BasicTurbulenceModel::correct();

		volScalarField::Internal divU
		(
			fvc::div(fvc::absolute(this->phi(), U))()()
		);

		tmp<volTensorField> tgradU = fvc::grad(U);
		volScalarField S2(2 * magSqr(symm(tgradU())));
		volScalarField::Internal GbyNu(dev(twoSymm(tgradU()())) && tgradU()());
		volScalarField::Internal G(this->GName(), nut()*GbyNu);
		tgradU.clear();

		// Update omega and G at the wall
		omega_.boundaryFieldRef().updateCoeffs();

		volScalarField CDkOmega
		(
			(2 * alphaOmega2_)*(fvc::grad(k_) & fvc::grad(omega_)) / omega_
		);

		volScalarField F1(this->F1(CDkOmega));
		volScalarField F23(this->F23());

		{
			volScalarField::Internal gamma(this->gamma(F1));
			volScalarField::Internal beta(this->beta(F1));

			// Turbulent frequency equation
			tmp<fvScalarMatrix> omegaEqn
			(
				fvm::ddt(alpha, rho, omega_)
				+ fvm::div(alphaRhoPhi, omega_)
				- fvm::laplacian(alpha*rho*DomegaEff(F1), omega_)
				==
				alpha()*rho()*gamma
				*min
				(
					GbyNu,
					(c1_ / a1_)*betaStar_*omega_()
					*max(a1_*omega_(), b1_*F23()*sqrt(S2()))
				)
				- fvm::SuSp((2.0 / 3.0)*alpha()*rho()*gamma*divU, omega_)
				- fvm::Sp(alpha()*rho()*beta*omega_(), omega_)
				- fvm::SuSp
				(
					alpha()*rho()*(F1() - scalar(1))*CDkOmega() / omega_(),
					omega_
				)
				+ Qsas(S2(), gamma, beta)
				+ omegaSource()
				+ fvOptions(alpha, rho, omega_)
			);

			omegaEqn.ref().relax();
			fvOptions.constrain(omegaEqn.ref());
			omegaEqn.ref().boundaryManipulate(omega_.boundaryFieldRef());
			solve(omegaEqn);
			fvOptions.correct(omega_);
			bound(omega_, this->omegaMin_);
		}

		// Turbulent kinetic energy equation
		tmp<fvScalarMatrix> kEqn
		(
			fvm::ddt(alpha, rho, k_)
			+ fvm::div(alphaRhoPhi, k_)
			- fvm::laplacian(alpha*rho*DkEff(F1), k_)
			==
			alpha()*rho()*Pk(G)
			- fvm::SuSp((2.0 / 3.0)*alpha()*rho()*divU, k_)
			- fvm::Sp(alpha()*rho()*epsilonByk(F1, F23), k_)
			+ kSource()
			+ fvOptions(alpha, rho, k_)
		);

		kEqn.ref().relax();
		fvOptions.constrain(kEqn.ref());
		solve(kEqn);
		fvOptions.correct(k_);
		bound(k_, this->kMin_);

		correctNut(S2, F23);
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //