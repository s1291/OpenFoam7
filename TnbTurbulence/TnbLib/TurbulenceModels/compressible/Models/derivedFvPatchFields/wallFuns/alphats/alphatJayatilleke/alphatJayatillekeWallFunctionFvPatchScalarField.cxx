#include <alphatJayatillekeWallFunctionFvPatchScalarField.hxx>

#include <turbulentFluidThermoModel.hxx>
#include <addToRunTimeSelectionTable.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace compressible
	{

		// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

		scalar alphatJayatillekeWallFunctionFvPatchScalarField::maxExp_ = 50.0;
		scalar alphatJayatillekeWallFunctionFvPatchScalarField::tolerance_ = 0.01;
		label alphatJayatillekeWallFunctionFvPatchScalarField::maxIters_ = 10;

		// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

		scalar alphatJayatillekeWallFunctionFvPatchScalarField::Psmooth
		(
			const scalar Prat
		) const
		{
			return 9.24*(pow(Prat, 0.75) - 1.0)*(1.0 + 0.28*exp(-0.007*Prat));
		}


		scalar alphatJayatillekeWallFunctionFvPatchScalarField::yPlusTherm
		(
			const nutWallFunctionFvPatchScalarField& nutw,
			const scalar P,
			const scalar Prat
		) const
		{
			scalar ypt = 11.0;

			for (int i = 0; i < maxIters_; i++)
			{
				scalar f = ypt - (log(nutw.E()*ypt) / nutw.kappa() + P) / Prat;
				scalar df = 1.0 - 1.0 / (ypt*nutw.kappa()*Prat);
				scalar yptNew = ypt - f / df;

				if (yptNew < vSmall)
				{
					return 0;
				}
				else if (mag(yptNew - ypt) < tolerance_)
				{
					return yptNew;
				}
				else
				{
					ypt = yptNew;
				}
			}

			return ypt;
		}


		// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

		alphatJayatillekeWallFunctionFvPatchScalarField::
			alphatJayatillekeWallFunctionFvPatchScalarField
			(
				const fvPatch& p,
				const DimensionedField<scalar, volMesh>& iF
			)
			:
			fixedValueFvPatchScalarField(p, iF),
			Prt_(0.85)
		{}


		alphatJayatillekeWallFunctionFvPatchScalarField::
			alphatJayatillekeWallFunctionFvPatchScalarField
			(
				const alphatJayatillekeWallFunctionFvPatchScalarField& ptf,
				const fvPatch& p,
				const DimensionedField<scalar, volMesh>& iF,
				const fvPatchFieldMapper& mapper
			)
			:
			fixedValueFvPatchScalarField(ptf, p, iF, mapper),
			Prt_(ptf.Prt_)
		{}


		alphatJayatillekeWallFunctionFvPatchScalarField::
			alphatJayatillekeWallFunctionFvPatchScalarField
			(
				const fvPatch& p,
				const DimensionedField<scalar, volMesh>& iF,
				const dictionary& dict
			)
			:
			fixedValueFvPatchScalarField(p, iF, dict),
			Prt_(dict.lookupOrDefault<scalar>("Prt", 0.85))
		{}


		alphatJayatillekeWallFunctionFvPatchScalarField::
			alphatJayatillekeWallFunctionFvPatchScalarField
			(
				const alphatJayatillekeWallFunctionFvPatchScalarField& awfpsf
			)
			:
			fixedValueFvPatchScalarField(awfpsf),
			Prt_(awfpsf.Prt_)
		{}


		alphatJayatillekeWallFunctionFvPatchScalarField::
			alphatJayatillekeWallFunctionFvPatchScalarField
			(
				const alphatJayatillekeWallFunctionFvPatchScalarField& awfpsf,
				const DimensionedField<scalar, volMesh>& iF
			)
			:
			fixedValueFvPatchScalarField(awfpsf, iF),
			Prt_(awfpsf.Prt_)
		{}


		// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

		void alphatJayatillekeWallFunctionFvPatchScalarField::updateCoeffs()
		{
			if (updated())
			{
				return;
			}

			const label patchi = patch().index();

			// Retrieve turbulence properties from model
			const compressible::turbulenceModel& turbModel =
				db().lookupObject<compressible::turbulenceModel>
				(
					IOobject::groupName
					(
						compressible::turbulenceModel::propertiesName,
						internalField().group()
					)
					);

			const nutWallFunctionFvPatchScalarField& nutw =
				nutWallFunctionFvPatchScalarField::nutw(turbModel, patchi);

			const scalar Cmu25 = pow025(nutw.Cmu());

			const scalarField& y = turbModel.y()[patchi];

			const tmp<scalarField> tmuw = turbModel.mu(patchi);
			const scalarField& muw = tmuw();

			const tmp<scalarField> talphaw = turbModel.alpha(patchi);
			const scalarField& alphaw = talphaw();

			scalarField& alphatw = *this;

			const tmp<volScalarField> tk = turbModel.k();
			const volScalarField& k = tk();

			const fvPatchVectorField& Uw = turbModel.U().boundaryField()[patchi];
			const scalarField magUp(mag(Uw.patchInternalField() - Uw));
			const scalarField magGradUw(mag(Uw.snGrad()));

			const scalarField& rhow = turbModel.rho().boundaryField()[patchi];
			const fvPatchScalarField& hew =
				turbModel.transport().he().boundaryField()[patchi];

			// Heat flux [W/m^2] - lagging alphatw
			const scalarField qDot
			(
				turbModel.transport().alphaEff(alphatw, patchi)*hew.snGrad()
			);

			// Populate boundary values
			forAll(alphatw, facei)
			{
				label celli = patch().faceCells()[facei];

				scalar uTau = Cmu25 * sqrt(k[celli]);

				scalar yPlus = uTau * y[facei] / (muw[facei] / rhow[facei]);

				// Molecular Prandtl number
				scalar Pr = muw[facei] / alphaw[facei];

				// Molecular-to-turbulent Prandtl number ratio
				scalar Prat = Pr / Prt_;

				// Thermal sublayer thickness
				scalar P = Psmooth(Prat);
				scalar yPlusTherm = this->yPlusTherm(nutw, P, Prat);

				// Evaluate new effective thermal diffusivity
				scalar alphaEff = 0.0;
				if (yPlus < yPlusTherm)
				{
					const scalar A = qDot[facei] * rhow[facei] * uTau*y[facei];

					const scalar B = qDot[facei] * Pr*yPlus;

					const scalar C = Pr * 0.5*rhow[facei] * uTau*sqr(magUp[facei]);

					alphaEff = A / (B + C + vSmall);
				}
				else
				{
					const scalar A = qDot[facei] * rhow[facei] * uTau*y[facei];

					const scalar B =
						qDot[facei] * Prt_*(1.0 / nutw.kappa()*log(nutw.E()*yPlus) + P);

					const scalar magUc =
						uTau / nutw.kappa()*log(nutw.E()*yPlusTherm) - mag(Uw[facei]);

					const scalar C =
						0.5*rhow[facei] * uTau
						*(Prt_*sqr(magUp[facei]) + (Pr - Prt_)*sqr(magUc));

					alphaEff = A / (B + C + vSmall);
				}

				// Update turbulent thermal diffusivity
				alphatw[facei] = max(0.0, alphaEff - alphaw[facei]);

				if (debug)
				{
					Info << "    uTau           = " << uTau << nl
						<< "    Pr             = " << Pr << nl
						<< "    Prt            = " << Prt_ << nl
						<< "    qDot           = " << qDot[facei] << nl
						<< "    yPlus          = " << yPlus << nl
						<< "    yPlusTherm     = " << yPlusTherm << nl
						<< "    alphaEff       = " << alphaEff << nl
						<< "    alphaw         = " << alphaw[facei] << nl
						<< "    alphatw        = " << alphatw[facei] << nl
						<< endl;
				}
			}

			fixedValueFvPatchField<scalar>::updateCoeffs();
		}


		void alphatJayatillekeWallFunctionFvPatchScalarField::write(Ostream& os) const
		{
			fvPatchField<scalar>::write(os);
			writeEntry(os, "Prt", Prt_);
			writeEntry(os, "value", *this);
		}


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		makePatchTypeField
		(
			fvPatchScalarField,
			alphatJayatillekeWallFunctionFvPatchScalarField
		);

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace compressible
} // End namespace tnbLib

// ************************************************************************* //