#include <kinematicSingleLayer.hxx>

#include <fvm.hxx>
#include <fvcDiv.hxx>
#include <fvcLaplacian.hxx>
#include <fvcSnGrad.hxx>
#include <fvcReconstruct.hxx>
#include <fvcVolumeIntegrate.hxx>
#include <fvcFlux.hxx>
#include <addToRunTimeSelectionTable.hxx>
#include <mappedWallPolyPatch.hxx>
#include <mapDistribute.hxx>
#include <filmThermoModel.hxx>

#include <zeroGradientFvPatchFields.hxx>  // added by amir
#include <PstreamReduceOps.hxx>  // added by amir

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace regionModels
	{
		namespace surfaceFilmModels
		{

			// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

			defineTypeNameAndDebug(kinematicSingleLayer, 0);

			addToRunTimeSelectionTable(surfaceFilmRegionModel, kinematicSingleLayer, mesh);

			// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

			bool kinematicSingleLayer::read()
			{
				if (surfaceFilmRegionModel::read())
				{
					const dictionary& solution = this->solution().subDict("PISO");
					solution.lookup("momentumPredictor") >> momentumPredictor_;
					solution.readIfPresent("nOuterCorr", nOuterCorr_);
					solution.lookup("nCorr") >> nCorr_;
					solution.lookup("nNonOrthCorr") >> nNonOrthCorr_;

					return true;
				}
				else
				{
					return false;
				}
			}


			void kinematicSingleLayer::correctThermoFields()
			{
				rho_ == filmThermo_->rho();
				mu_ == filmThermo_->mu();
				sigma_ == filmThermo_->sigma();
			}


			void kinematicSingleLayer::resetPrimaryRegionSourceTerms()
			{
				if (debug)
				{
					InfoInFunction << endl;
				}

				rhoSpPrimary_ == dimensionedScalar(rhoSp_.dimensions(), 0);
				USpPrimary_ == dimensionedVector(USp_.dimensions(), Zero);
				pSpPrimary_ == dimensionedScalar(pSp_.dimensions(), 0);
			}


			void kinematicSingleLayer::transferPrimaryRegionThermoFields()
			{
				if (debug)
				{
					InfoInFunction << endl;
				}

				// Update fields from primary region via direct mapped
				// (coupled) boundary conditions
				UPrimary_.correctBoundaryConditions();
				pPrimary_.correctBoundaryConditions();
				rhoPrimary_.correctBoundaryConditions();
				muPrimary_.correctBoundaryConditions();
			}


			void kinematicSingleLayer::transferPrimaryRegionSourceFields()
			{
				if (debug)
				{
					InfoInFunction << endl;
				}

				volScalarField::Boundary& rhoSpPrimaryBf =
					rhoSpPrimary_.boundaryFieldRef();

				volVectorField::Boundary& USpPrimaryBf =
					USpPrimary_.boundaryFieldRef();

				volScalarField::Boundary& pSpPrimaryBf =
					pSpPrimary_.boundaryFieldRef();

				// Convert accummulated source terms into per unit area per unit time
				const scalar deltaT = time_.deltaTValue();
				forAll(rhoSpPrimary_.boundaryField(), patchi)
				{
					scalarField rpriMagSfdeltaT
					(
						(1.0 / deltaT)
						/ primaryMesh().magSf().boundaryField()[patchi]
					);

					rhoSpPrimaryBf[patchi] *= rpriMagSfdeltaT;
					USpPrimaryBf[patchi] *= rpriMagSfdeltaT;
					pSpPrimaryBf[patchi] *= rpriMagSfdeltaT;
				}

				// Retrieve the source fields from the primary region via direct mapped
				// (coupled) boundary conditions
				// - fields require transfer of values for both patch AND to push the
				//   values into the first layer of internal cells
				rhoSp_.correctBoundaryConditions();
				USp_.correctBoundaryConditions();
				pSp_.correctBoundaryConditions();

				// update addedMassTotal counter
				if (time().writeTime())
				{
					scalar addedMassTotal = 0.0;
					outputProperties().readIfPresent("addedMassTotal", addedMassTotal);
					addedMassTotal += returnReduce(addedMassTotal_, sumOp<scalar>());
					outputProperties().add("addedMassTotal", addedMassTotal, true);
					addedMassTotal_ = 0.0;
				}
			}


			tmp<volScalarField> kinematicSingleLayer::pu()
			{
				return volScalarField::New
				(
					typeName + ":pu",
					pPrimary_                      // pressure (mapped from primary region)
					- pSp_                           // accumulated particle impingement
					- fvc::laplacian(sigma_, delta_) // surface tension
				);
			}


			tmp<volScalarField> kinematicSingleLayer::pp()
			{
				return volScalarField::New
				(
					typeName + ":pp",
					-rho_ * gNormClipped() // hydrostatic effect only
				);
			}


			void kinematicSingleLayer::correctAlpha()
			{
				alpha_ == pos(delta_ - deltaSmall_);
			}


			void kinematicSingleLayer::updateSubmodels()
			{
				if (debug)
				{
					InfoInFunction << endl;
				}

				// Update injection model - mass returned is mass available for injection
				injection_.correct(availableMass_, cloudMassTrans_, cloudDiameterTrans_);

				// Update transfer model - mass returned is mass available for transfer
				transfer_.correct(availableMass_, cloudMassTrans_);

				// Update mass source field
				rhoSp_ += cloudMassTrans_ / magSf() / time().deltaT();

				turbulence_->correct();
			}


			void kinematicSingleLayer::continuityCheck()
			{
				const volScalarField deltaRho0(deltaRho_);

				solveContinuity();

				if (debug)
				{
					const volScalarField mass(deltaRho_*magSf());
					const dimensionedScalar totalMass =
						fvc::domainIntegrate(mass)
						+ dimensionedScalar(dimMass*dimVolume, rootVSmall);

					const scalar sumLocalContErr =
						(
							fvc::domainIntegrate(mag(mass - magSf()*deltaRho0)) / totalMass
							).value();

					const scalar globalContErr =
						(
							fvc::domainIntegrate(mass - magSf()*deltaRho0) / totalMass
							).value();

					cumulativeContErr_ += globalContErr;

					InfoInFunction
						<< "Surface film: " << type() << nl
						<< "    time step continuity errors: sum local = "
						<< sumLocalContErr << ", global = " << globalContErr
						<< ", cumulative = " << cumulativeContErr_ << endl;
				}
			}


			void kinematicSingleLayer::solveContinuity()
			{
				if (debug)
				{
					InfoInFunction << endl;
				}

				solve
				(
					fvm::ddt(deltaRho_)
					+ fvc::div(phi_)
					==
					-rhoSp_
				);
			}


			void kinematicSingleLayer::updateSurfaceVelocities()
			{
				// Push boundary film velocity values into internal field
				for (label i = 0; i < intCoupledPatchIDs_.size(); i++)
				{
					label patchi = intCoupledPatchIDs_[i];
					const polyPatch& pp = regionMesh().boundaryMesh()[patchi];
					UIndirectList<vector>(Uw_, pp.faceCells()) =
						U_.boundaryField()[patchi];
				}
				Uw_ -= nHat()*(Uw_ & nHat());
				Uw_.correctBoundaryConditions();

				Us_ = turbulence_->Us();
			}


			tmp<tnbLib::fvVectorMatrix> kinematicSingleLayer::solveMomentum
			(
				const volScalarField& pu,
				const volScalarField& pp
			)
			{
				if (debug)
				{
					InfoInFunction << endl;
				}

				// Momentum
				tmp<fvVectorMatrix> tUEqn
				(
					fvm::ddt(deltaRho_, U_)
					+ fvm::div(phi_, U_)
					==
					-USp_
					// - fvm::SuSp(rhoSp_, U_)
					- rhoSp_ * U_
					+ forces_.correct(U_)
					+ turbulence_->Su(U_)
				);

				fvVectorMatrix& UEqn = tUEqn.ref();

				UEqn.relax();

				if (momentumPredictor_)
				{
					solve
					(
						UEqn
						==
						fvc::reconstruct
						(
							-fvc::interpolate(delta_)
							* (
								regionMesh().magSf()
								* (
									fvc::snGrad(pu, "snGrad(p)")
									+ fvc::snGrad(pp, "snGrad(p)")*fvc::interpolate(delta_)
									+ fvc::snGrad(delta_)*fvc::interpolate(pp)
									)
								- fvc::flux(rho_*gTan())
								)
						)
					);

					// Remove any patch-normal components of velocity
					U_ -= nHat()*(nHat() & U_);
					U_.correctBoundaryConditions();
				}

				return tUEqn;
			}


			void kinematicSingleLayer::solveThickness
			(
				const volScalarField& pu,
				const volScalarField& pp,
				const fvVectorMatrix& UEqn
			)
			{
				if (debug)
				{
					InfoInFunction << endl;
				}

				volScalarField rUA(1.0 / UEqn.A());
				U_ = rUA * UEqn.H();

				surfaceScalarField deltarUAf(fvc::interpolate(delta_*rUA));
				surfaceScalarField rhof(fvc::interpolate(rho_));

				surfaceScalarField phiAdd
				(
					"phiAdd",
					regionMesh().magSf()
					* (
						fvc::snGrad(pu, "snGrad(p)")
						+ fvc::snGrad(pp, "snGrad(p)")*fvc::interpolate(delta_)
						)
					- fvc::flux(rho_*gTan())
				);
				constrainFilmField(phiAdd, 0.0);

				surfaceScalarField phid
				(
					"phid",
					fvc::flux(U_*rho_) - deltarUAf * phiAdd*rhof
				);
				constrainFilmField(phid, 0.0);

				surfaceScalarField ddrhorUAppf
				(
					"deltaCoeff",
					fvc::interpolate(delta_)*deltarUAf*rhof*fvc::interpolate(pp)
				);

				regionMesh().setFluxRequired(delta_.name());

				for (int nonOrth = 0; nonOrth <= nNonOrthCorr_; nonOrth++)
				{
					// Film thickness equation
					fvScalarMatrix deltaEqn
					(
						fvm::ddt(rho_, delta_)
						+ fvm::div(phid, delta_)
						- fvm::laplacian(ddrhorUAppf, delta_)
						==
						-rhoSp_
					);

					deltaEqn.solve();

					if (nonOrth == nNonOrthCorr_)
					{
						phiAdd +=
							fvc::interpolate(pp)
							* fvc::snGrad(delta_)
							* regionMesh().magSf();

						phi_ == deltaEqn.flux();
					}
				}

				// Bound film thickness by a minimum of zero
				delta_.max(0.0);

				// Update U field
				U_ -= fvc::reconstruct(deltarUAf*phiAdd);

				// Remove any patch-normal components of velocity
				U_ -= nHat()*(nHat() & U_);

				U_.correctBoundaryConditions();

				// Continuity check
				continuityCheck();
			}


			// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

			kinematicSingleLayer::kinematicSingleLayer
			(
				const word& modelType,
				const fvMesh& mesh,
				const dimensionedVector& g,
				const word& regionType,
				const bool readFields
			)
				:
				surfaceFilmRegionModel(modelType, mesh, g, regionType),

				momentumPredictor_(solution().subDict("PISO").lookup("momentumPredictor")),
				nOuterCorr_(solution().subDict("PISO").lookupOrDefault("nOuterCorr", 1)),
				nCorr_(readLabel(solution().subDict("PISO").lookup("nCorr"))),
				nNonOrthCorr_
				(
					readLabel(solution().subDict("PISO").lookup("nNonOrthCorr"))
				),

				cumulativeContErr_(0.0),

				deltaSmall_("deltaSmall", dimLength, small),
				deltaCoLimit_(solution().lookupOrDefault("deltaCoLimit", 1e-4)),

				rho_
				(
					IOobject
					(
						"rhof",
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::AUTO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimDensity, 0),
					zeroGradientFvPatchScalarField::typeName
				),
				mu_
				(
					IOobject
					(
						"muf",
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::AUTO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimPressure*dimTime, 0),
					zeroGradientFvPatchScalarField::typeName
				),
				sigma_
				(
					IOobject
					(
						"sigmaf",
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::AUTO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimMass / sqr(dimTime), 0),
					zeroGradientFvPatchScalarField::typeName
				),

				delta_
				(
					IOobject
					(
						"deltaf",
						time().timeName(),
						regionMesh(),
						IOobject::MUST_READ,
						IOobject::AUTO_WRITE
					),
					regionMesh()
				),
				alpha_
				(
					IOobject
					(
						"alpha",
						time().timeName(),
						regionMesh(),
						IOobject::READ_IF_PRESENT,
						IOobject::AUTO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimless, 0),
					zeroGradientFvPatchScalarField::typeName
				),
				U_
				(
					IOobject
					(
						"Uf",
						time().timeName(),
						regionMesh(),
						IOobject::MUST_READ,
						IOobject::AUTO_WRITE
					),
					regionMesh()
				),
				Us_
				(
					IOobject
					(
						"Usf",
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					U_,
					zeroGradientFvPatchScalarField::typeName
				),
				Uw_
				(
					IOobject
					(
						"Uwf",
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					U_,
					zeroGradientFvPatchScalarField::typeName
				),
				deltaRho_
				(
					IOobject
					(
						delta_.name() + "*" + rho_.name(),
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					regionMesh(),
					dimensionedScalar(delta_.dimensions()*rho_.dimensions(), 0),
					zeroGradientFvPatchScalarField::typeName
				),

				phi_
				(
					IOobject
					(
						"phi",
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::AUTO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimLength*dimMass / dimTime, 0)
				),

				primaryMassTrans_
				(
					IOobject
					(
						"primaryMassTrans",
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimMass, 0),
					zeroGradientFvPatchScalarField::typeName
				),
				cloudMassTrans_
				(
					IOobject
					(
						"cloudMassTrans",
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimMass, 0),
					zeroGradientFvPatchScalarField::typeName
				),
				cloudDiameterTrans_
				(
					IOobject
					(
						"cloudDiameterTrans",
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimLength, -1.0),
					zeroGradientFvPatchScalarField::typeName
				),

				USp_
				(
					IOobject
					(
						"USpf",
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					regionMesh(),
					dimensionedVector
					(
						"zero", dimMass*dimVelocity / dimArea / dimTime, Zero
					),
					this->mappedPushedFieldPatchTypes<vector>()
				),
				pSp_
				(
					IOobject
					(
						"pSpf",
						time_.timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimPressure, 0),
					this->mappedPushedFieldPatchTypes<scalar>()
				),
				rhoSp_
				(
					IOobject
					(
						"rhoSpf",
						time_.timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimMass / dimTime / dimArea, 0),
					this->mappedPushedFieldPatchTypes<scalar>()
				),

				USpPrimary_
				(
					IOobject
					(
						USp_.name(), // must have same name as USp_ to enable mapping
						time().timeName(),
						primaryMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					primaryMesh(),
					dimensionedVector(USp_.dimensions(), Zero)
				),
				pSpPrimary_
				(
					IOobject
					(
						pSp_.name(), // must have same name as pSp_ to enable mapping
						time().timeName(),
						primaryMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					primaryMesh(),
					dimensionedScalar(pSp_.dimensions(), 0)
				),
				rhoSpPrimary_
				(
					IOobject
					(
						rhoSp_.name(), // must have same name as rhoSp_ to enable mapping
						time().timeName(),
						primaryMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					primaryMesh(),
					dimensionedScalar(rhoSp_.dimensions(), 0)
				),

				UPrimary_
				(
					IOobject
					(
						"U", // must have same name as U to enable mapping
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					regionMesh(),
					dimensionedVector(dimVelocity, Zero),
					this->mappedFieldAndInternalPatchTypes<vector>()
				),
				pPrimary_
				(
					IOobject
					(
						"p", // must have same name as p to enable mapping
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimPressure, 0),
					this->mappedFieldAndInternalPatchTypes<scalar>()
				),
				rhoPrimary_
				(
					IOobject
					(
						"rho", // must have same name as rho to enable mapping
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimDensity, 0),
					this->mappedFieldAndInternalPatchTypes<scalar>()
				),
				muPrimary_
				(
					IOobject
					(
						"thermo:mu", // must have same name as mu to enable mapping
						time().timeName(),
						regionMesh(),
						IOobject::NO_READ,
						IOobject::NO_WRITE
					),
					regionMesh(),
					dimensionedScalar(dimPressure*dimTime, 0),
					this->mappedFieldAndInternalPatchTypes<scalar>()
				),

				filmThermo_(filmThermoModel::New(*this, coeffs_)),

				availableMass_(regionMesh().nCells(), 0.0),

				injection_(*this, coeffs_),

				transfer_(*this, coeffs_),

				turbulence_(filmTurbulenceModel::New(*this, coeffs_)),

				forces_(*this, coeffs_),

				addedMassTotal_(0.0)
			{
				if (readFields)
				{
					transferPrimaryRegionThermoFields();

					correctAlpha();

					correctThermoFields();

					deltaRho_ == delta_ * rho_;

					surfaceScalarField phi0
					(
						IOobject
						(
							"phi",
							time().timeName(),
							regionMesh(),
							IOobject::READ_IF_PRESENT,
							IOobject::AUTO_WRITE,
							false
						),
						fvc::flux(deltaRho_*U_)
					);

					phi_ == phi0;
				}
			}


			// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

			kinematicSingleLayer::~kinematicSingleLayer()
			{}


			// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

			void kinematicSingleLayer::addSources
			(
				const label patchi,
				const label facei,
				const scalar massSource,
				const vector& momentumSource,
				const scalar pressureSource,
				const scalar energySource
			)
			{
				if (debug)
				{
					InfoInFunction
						<< "\nSurface film: " << type() << ": adding to film source:" << nl
						<< "    mass     = " << massSource << nl
						<< "    momentum = " << momentumSource << nl
						<< "    pressure = " << pressureSource << endl;
				}

				rhoSpPrimary_.boundaryFieldRef()[patchi][facei] -= massSource;
				USpPrimary_.boundaryFieldRef()[patchi][facei] -= momentumSource;
				pSpPrimary_.boundaryFieldRef()[patchi][facei] -= pressureSource;

				addedMassTotal_ += massSource;
			}


			void kinematicSingleLayer::preEvolveRegion()
			{
				if (debug)
				{
					InfoInFunction << endl;
				}

				surfaceFilmRegionModel::preEvolveRegion();

				transferPrimaryRegionThermoFields();

				correctThermoFields();

				transferPrimaryRegionSourceFields();

				// Reset transfer fields
				availableMass_ = mass();
				cloudMassTrans_ == dimensionedScalar(dimMass, 0);
				cloudDiameterTrans_ == dimensionedScalar(dimLength, 0);
				primaryMassTrans_ == dimensionedScalar(dimMass, 0);
			}


			void kinematicSingleLayer::evolveRegion()
			{
				if (debug)
				{
					InfoInFunction << endl;
				}

				// Update film coverage indicator
				correctAlpha();

				// Update film wall and surface velocities
				updateSurfaceVelocities();

				// Update sub-models to provide updated source contributions
				updateSubmodels();

				// Solve continuity for deltaRho_
				solveContinuity();

				// Implicit pressure source coefficient - constant
				tmp<volScalarField> tpp(this->pp());

				for (int oCorr = 1; oCorr <= nOuterCorr_; oCorr++)
				{
					// Explicit pressure source contribution - varies with delta_
					tmp<volScalarField> tpu(this->pu());

					// Solve for momentum for U_
					tmp<fvVectorMatrix> UEqn = solveMomentum(tpu(), tpp());

					// Film thickness correction loop
					for (int corr = 1; corr <= nCorr_; corr++)
					{
						// Solve thickness for delta_
						solveThickness(tpu(), tpp(), UEqn());
					}
				}

				// Update deltaRho_ with new delta_
				deltaRho_ == delta_ * rho_;

				// Reset source terms for next time integration
				resetPrimaryRegionSourceTerms();
			}


			scalar kinematicSingleLayer::CourantNumber() const
			{
				scalar CoNum = 0.0;

				if (regionMesh().nInternalFaces() > 0)
				{
					const scalarField sumPhi
					(
						fvc::surfaceSum(mag(phi_))().primitiveField()
						/ (deltaRho_.primitiveField() + rootVSmall)
					);

					forAll(delta_, i)
					{
						if (delta_[i] > deltaCoLimit_)
						{
							CoNum = max(CoNum, sumPhi[i] / (delta_[i] * magSf()[i]));
						}
					}

					CoNum *= 0.5*time_.deltaTValue();
				}

				reduce(CoNum, maxOp<scalar>());

				Info << "Film max Courant number: " << CoNum << endl;

				return CoNum;
			}


			const volVectorField& kinematicSingleLayer::U() const
			{
				return U_;
			}


			const volVectorField& kinematicSingleLayer::Us() const
			{
				return Us_;
			}


			const volVectorField& kinematicSingleLayer::Uw() const
			{
				return Uw_;
			}


			const volScalarField& kinematicSingleLayer::deltaRho() const
			{
				return deltaRho_;
			}


			const surfaceScalarField& kinematicSingleLayer::phi() const
			{
				return phi_;
			}


			const volScalarField& kinematicSingleLayer::rho() const
			{
				return rho_;
			}


			const volScalarField& kinematicSingleLayer::T() const
			{
				FatalErrorInFunction
					<< "T field not available for " << type() << abort(FatalError);

				return volScalarField::null();
			}


			const volScalarField& kinematicSingleLayer::Ts() const
			{
				FatalErrorInFunction
					<< "Ts field not available for " << type() << abort(FatalError);

				return volScalarField::null();
			}


			const volScalarField& kinematicSingleLayer::Tw() const
			{
				FatalErrorInFunction
					<< "Tw field not available for " << type() << abort(FatalError);

				return volScalarField::null();
			}


			const volScalarField& kinematicSingleLayer::hs() const
			{
				FatalErrorInFunction
					<< "hs field not available for " << type() << abort(FatalError);

				return volScalarField::null();
			}


			const volScalarField& kinematicSingleLayer::Cp() const
			{
				FatalErrorInFunction
					<< "Cp field not available for " << type() << abort(FatalError);

				return volScalarField::null();
			}


			const volScalarField& kinematicSingleLayer::kappa() const
			{
				FatalErrorInFunction
					<< "kappa field not available for " << type() << abort(FatalError);

				return volScalarField::null();
			}


			tmp<volScalarField> kinematicSingleLayer::primaryMassTrans() const
			{
				return primaryMassTrans_;
			}


			const volScalarField& kinematicSingleLayer::cloudMassTrans() const
			{
				return cloudMassTrans_;
			}


			const volScalarField& kinematicSingleLayer::cloudDiameterTrans() const
			{
				return cloudDiameterTrans_;
			}


			void kinematicSingleLayer::info()
			{
				Info << "\nSurface film: " << type() << endl;

				const scalarField& deltaInternal = delta_;
				const vectorField& Uinternal = U_;
				scalar addedMassTotal = 0.0;
				outputProperties().readIfPresent("addedMassTotal", addedMassTotal);
				addedMassTotal += returnReduce(addedMassTotal_, sumOp<scalar>());

				Info << indent << "added mass         = " << addedMassTotal << nl
					<< indent << "current mass       = "
					<< gSum((deltaRho_*magSf())()) << nl
					<< indent << "min/max(mag(U))    = " << gMin(mag(Uinternal)) << ", "
					<< gMax(mag(Uinternal)) << nl
					<< indent << "min/max(delta)     = " << gMin(deltaInternal) << ", "
					<< gMax(deltaInternal) << nl
					<< indent << "coverage           = "
					<< gSum(alpha_.primitiveField()*magSf()) / gSum(magSf()) << nl;

				injection_.info(Info);
				transfer_.info(Info);
			}


			tmp<volScalarField::Internal> kinematicSingleLayer::Srho() const
			{
				return volScalarField::Internal::New
				(
					typeName + ":Srho",
					primaryMesh(),
					dimensionedScalar(dimMass / dimVolume / dimTime, 0)
				);
			}


			tmp<volScalarField::Internal> kinematicSingleLayer::Srho
			(
				const label i
			) const
			{
				return volScalarField::Internal::New
				(
					typeName + ":Srho(" + tnbLib::name(i) + ")",
					primaryMesh(),
					dimensionedScalar(dimMass / dimVolume / dimTime, 0)
				);
			}


			tmp<volScalarField::Internal> kinematicSingleLayer::Sh() const
			{
				return volScalarField::Internal::New
				(
					typeName + ":Sh",
					primaryMesh(),
					dimensionedScalar(dimEnergy / dimVolume / dimTime, 0)
				);
			}


			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		} // End namespace surfaceFilmModels
	} // End namespace regionModels
} // End namespace tnbLib

// ************************************************************************* //