#include <fvDOM.hxx>

#include <absorptionEmissionModel.hxx>
#include <scatterModel.hxx>
#include <constants.hxx>
#include <fvm.hxx>
#include <addToRunTimeSelectionTable.hxx>

#include <Time.hxx>  // added by amir
#include <IStringStream.hxx>  // added by amir

using namespace tnbLib::constant;
using namespace tnbLib::constant::mathematical;

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace radiationModels
	{
		defineTypeNameAndDebug(fvDOM, 0);
		addToRadiationRunTimeSelectionTables(fvDOM);
	}
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void tnbLib::radiationModels::fvDOM::initialise()
{
	// 3D
	if (mesh_.nSolutionD() == 3)
	{
		nRay_ = 4 * nPhi_*nTheta_;
		IRay_.setSize(nRay_);
		const scalar deltaPhi = pi / (2.0*nPhi_);
		const scalar deltaTheta = pi / nTheta_;
		label i = 0;
		for (label n = 1; n <= nTheta_; n++)
		{
			for (label m = 1; m <= 4 * nPhi_; m++)
			{
				const scalar thetai = (2 * n - 1)*deltaTheta / 2.0;
				const scalar phii = (2 * m - 1)*deltaPhi / 2.0;
				IRay_.set
				(
					i,
					new radiativeIntensityRay
					(
						*this,
						mesh_,
						phii,
						thetai,
						deltaPhi,
						deltaTheta,
						nLambda_,
						absorptionEmission_,
						blackBody_,
						i
					)
				);
				i++;
			}
		}
	}
	// 2D
	else if (mesh_.nSolutionD() == 2)
	{
		const scalar thetai = piByTwo;
		const scalar deltaTheta = pi;
		nRay_ = 4 * nPhi_;
		IRay_.setSize(nRay_);
		const scalar deltaPhi = pi / (2.0*nPhi_);
		label i = 0;
		for (label m = 1; m <= 4 * nPhi_; m++)
		{
			const scalar phii = (2 * m - 1)*deltaPhi / 2.0;
			IRay_.set
			(
				i,
				new radiativeIntensityRay
				(
					*this,
					mesh_,
					phii,
					thetai,
					deltaPhi,
					deltaTheta,
					nLambda_,
					absorptionEmission_,
					blackBody_,
					i
				)
			);
			i++;
		}
	}
	// 1D
	else
	{
		const scalar thetai = piByTwo;
		const scalar deltaTheta = pi;
		nRay_ = 2;
		IRay_.setSize(nRay_);
		const scalar deltaPhi = pi;
		label i = 0;
		for (label m = 1; m <= 2; m++)
		{
			const scalar phii = (2 * m - 1)*deltaPhi / 2.0;
			IRay_.set
			(
				i,
				new radiativeIntensityRay
				(
					*this,
					mesh_,
					phii,
					thetai,
					deltaPhi,
					deltaTheta,
					nLambda_,
					absorptionEmission_,
					blackBody_,
					i
				)
			);
			i++;
		}
	}


	// Construct absorption field for each wavelength
	forAll(aLambda_, lambdaI)
	{
		aLambda_.set
		(
			lambdaI,
			new volScalarField
			(
				IOobject
				(
					"aLambda_" + tnbLib::name(lambdaI),
					mesh_.time().timeName(),
					mesh_,
					IOobject::NO_READ,
					IOobject::NO_WRITE
				),
				a_
			)
		);
	}


	// Calculate the maximum solid angle
	forAll(IRay_, rayId)
	{
		if (omegaMax_ < IRay_[rayId].omega())
		{
			omegaMax_ = IRay_[rayId].omega();
		}
	}


	Info << typeName << ": Created " << IRay_.size() << " rays with average "
		<< "directions (dAve) and solid angles (omega)" << endl;
	Info << incrIndent;
	forAll(IRay_, rayId)
	{
		Info << indent
			<< "Ray " << IRay_[rayId].I().name() << ": "
			<< "dAve = " << IRay_[rayId].dAve() << ", "
			<< "omega = " << IRay_[rayId].omega() << endl;
	}
	Info << decrIndent << endl;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tnbLib::radiationModels::fvDOM::fvDOM(const volScalarField& T)
	:
	radiationModel(typeName, T),
	G_
	(
		IOobject
		(
			"G",
			mesh_.time().timeName(),
			mesh_,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		mesh_,
		dimensionedScalar(dimMass / pow3(dimTime), 0)
	),
	qr_
	(
		IOobject
		(
			"qr",
			mesh_.time().timeName(),
			mesh_,
			IOobject::READ_IF_PRESENT,
			IOobject::AUTO_WRITE
		),
		mesh_,
		dimensionedScalar(dimMass / pow3(dimTime), 0)
	),
	qem_
	(
		IOobject
		(
			"qem",
			mesh_.time().timeName(),
			mesh_,
			IOobject::READ_IF_PRESENT,
			IOobject::AUTO_WRITE
		),
		mesh_,
		dimensionedScalar(dimMass / pow3(dimTime), 0)
	),
	qin_
	(
		IOobject
		(
			"qin",
			mesh_.time().timeName(),
			mesh_,
			IOobject::READ_IF_PRESENT,
			IOobject::AUTO_WRITE
		),
		mesh_,
		dimensionedScalar(dimMass / pow3(dimTime), 0)
	),
	a_
	(
		IOobject
		(
			"a",
			mesh_.time().timeName(),
			mesh_,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		mesh_,
		dimensionedScalar(dimless / dimLength, 0)
	),
	nTheta_(readLabel(coeffs_.lookup("nTheta"))),
	nPhi_(readLabel(coeffs_.lookup("nPhi"))),
	nRay_(0),
	nLambda_(absorptionEmission_->nBands()),
	aLambda_(nLambda_),
	blackBody_(nLambda_, T),
	IRay_(0),
	tolerance_
	(
		coeffs_.found("convergence")
		? readScalar(coeffs_.lookup("convergence"))
		: coeffs_.lookupOrDefault<scalar>("tolerance", 0)
	),
	maxIter_(coeffs_.lookupOrDefault<label>("maxIter", 50)),
	omegaMax_(0)
{
	initialise();
}


tnbLib::radiationModels::fvDOM::fvDOM
(
	const dictionary& dict,
	const volScalarField& T
)
	:
	radiationModel(typeName, dict, T),
	G_
	(
		IOobject
		(
			"G",
			mesh_.time().timeName(),
			mesh_,
			IOobject::READ_IF_PRESENT,
			IOobject::AUTO_WRITE
		),
		mesh_,
		dimensionedScalar(dimMass / pow3(dimTime), 0)
	),
	qr_
	(
		IOobject
		(
			"qr",
			mesh_.time().timeName(),
			mesh_,
			IOobject::READ_IF_PRESENT,
			IOobject::AUTO_WRITE
		),
		mesh_,
		dimensionedScalar(dimMass / pow3(dimTime), 0)
	),
	qem_
	(
		IOobject
		(
			"qem",
			mesh_.time().timeName(),
			mesh_,
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		mesh_,
		dimensionedScalar(dimMass / pow3(dimTime), 0)
	),
	qin_
	(
		IOobject
		(
			"qin",
			mesh_.time().timeName(),
			mesh_,
			IOobject::READ_IF_PRESENT,
			IOobject::AUTO_WRITE
		),
		mesh_,
		dimensionedScalar(dimMass / pow3(dimTime), 0)
	),
	a_
	(
		IOobject
		(
			"a",
			mesh_.time().timeName(),
			mesh_,
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		mesh_,
		dimensionedScalar(dimless / dimLength, 0)
	),
	nTheta_(readLabel(coeffs_.lookup("nTheta"))),
	nPhi_(readLabel(coeffs_.lookup("nPhi"))),
	nRay_(0),
	nLambda_(absorptionEmission_->nBands()),
	aLambda_(nLambda_),
	blackBody_(nLambda_, T),
	IRay_(0),
	tolerance_
	(
		coeffs_.found("convergence")
		? readScalar(coeffs_.lookup("convergence"))
		: coeffs_.lookupOrDefault<scalar>("tolerance", 0)
	),
	maxIter_(coeffs_.lookupOrDefault<label>("maxIter", 50)),
	omegaMax_(0)
{
	initialise();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

tnbLib::radiationModels::fvDOM::~fvDOM()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool tnbLib::radiationModels::fvDOM::read()
{
	if (radiationModel::read())
	{
		// Only reading solution parameters - not changing ray geometry

		// For backward-compatibility
		coeffs_.readIfPresent("convergence", tolerance_);
		coeffs_.readIfPresent("tolerance", tolerance_);
		coeffs_.readIfPresent("maxIter", maxIter_);

		return true;
	}
	else
	{
		return false;
	}
}


void tnbLib::radiationModels::fvDOM::calculate()
{
	absorptionEmission_->correct(a_, aLambda_);

	updateBlackBodyEmission();

	// Set rays converged false
	List<bool> rayIdConv(nRay_, false);

	scalar maxResidual = 0;
	label radIter = 0;
	do
	{
		Info << "Radiation solver iter: " << radIter << endl;

		radIter++;
		maxResidual = 0;
		forAll(IRay_, rayI)
		{
			if (!rayIdConv[rayI])
			{
				scalar maxBandResidual = IRay_[rayI].correct();
				maxResidual = max(maxBandResidual, maxResidual);

				if (maxBandResidual < tolerance_)
				{
					rayIdConv[rayI] = true;
				}
			}
		}

	} while (maxResidual > tolerance_ && radIter < maxIter_);

	updateG();
}


tnbLib::tmp<tnbLib::volScalarField> tnbLib::radiationModels::fvDOM::Rp() const
{
	// Construct using contribution from first frequency band
	tmp<volScalarField> tRp
	(
		volScalarField::New
		(
			"Rp",
			4
			* physicoChemical::sigma
			*(aLambda_[0] - absorptionEmission_->aDisp(0)())
			*blackBody_.deltaLambdaT(T_, absorptionEmission_->bands(0))
		)
	);

	volScalarField& Rp = tRp.ref();

	// Add contributions over remaining frequency bands
	for (label j = 1; j < nLambda_; j++)
	{
		Rp +=
			(
				4
				* physicoChemical::sigma
				*(aLambda_[j] - absorptionEmission_->aDisp(j)())
				*blackBody_.deltaLambdaT(T_, absorptionEmission_->bands(j))
				);
	}

	return tRp;
}


tnbLib::tmp<tnbLib::DimensionedField<tnbLib::scalar, tnbLib::volMesh>>
tnbLib::radiationModels::fvDOM::Ru() const
{
	tmp<DimensionedField<scalar, volMesh>> tRu
	(
		new DimensionedField<scalar, volMesh>
		(
			IOobject
			(
				"Ru",
				mesh_.time().timeName(),
				mesh_,
				IOobject::NO_READ,
				IOobject::NO_WRITE,
				false
			),
			mesh_,
			dimensionedScalar(dimensionSet(1, -1, -3, 0, 0), 0)
			)
	);

	DimensionedField<scalar, volMesh>& Ru = tRu.ref();

	// Sum contributions over all frequency bands
	for (label j = 0; j < nLambda_; j++)
	{
		// Compute total incident radiation within frequency band
		tmp<DimensionedField<scalar, volMesh>> Gj
		(
			IRay_[0].ILambda(j)()*IRay_[0].omega()
		);

		for (label rayI = 1; rayI < nRay_; rayI++)
		{
			Gj.ref() += IRay_[rayI].ILambda(j)()*IRay_[rayI].omega();
		}

		Ru += (aLambda_[j]() - absorptionEmission_->aDisp(j)()())*Gj
			- absorptionEmission_->ECont(j)()();
	}

	return tRu;
}


void tnbLib::radiationModels::fvDOM::updateBlackBodyEmission()
{
	for (label j = 0; j < nLambda_; j++)
	{
		blackBody_.correct(j, absorptionEmission_->bands(j));
	}
}


void tnbLib::radiationModels::fvDOM::updateG()
{
	G_ = dimensionedScalar("zero", dimMass / pow3(dimTime), 0);
	qr_ = dimensionedScalar("zero", dimMass / pow3(dimTime), 0);
	qem_ = dimensionedScalar(dimMass / pow3(dimTime), 0);
	qin_ = dimensionedScalar(dimMass / pow3(dimTime), 0);

	forAll(IRay_, rayI)
	{
		IRay_[rayI].addIntensity();
		G_ += IRay_[rayI].I()*IRay_[rayI].omega();
		qr_.boundaryFieldRef() += IRay_[rayI].qr().boundaryField();
		qem_.boundaryFieldRef() += IRay_[rayI].qem().boundaryField();
		qin_.boundaryFieldRef() += IRay_[rayI].qin().boundaryField();
	}
}


void tnbLib::radiationModels::fvDOM::setRayIdLambdaId
(
	const word& name,
	label& rayId,
	label& lambdaId
) const
{
	// Assume name is in the form: <name>_<rayId>_<lambdaId>
	const size_type i1 = name.find_first_of("_");
	const size_type i2 = name.find_last_of("_");

	rayId = readLabel(IStringStream(name.substr(i1 + 1, i2 - 1))());
	lambdaId = readLabel(IStringStream(name.substr(i2 + 1, name.size() - 1))());
}


// ************************************************************************* //