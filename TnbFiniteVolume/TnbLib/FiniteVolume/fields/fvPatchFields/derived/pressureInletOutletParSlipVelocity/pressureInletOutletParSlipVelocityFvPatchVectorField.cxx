#include <pressureInletOutletParSlipVelocityFvPatchVectorField.hxx>

#include <addToRunTimeSelectionTable.hxx>
#include <fvPatchFieldMapper.hxx>
#include <volFields.hxx>
#include <surfaceFields.hxx>

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tnbLib::pressureInletOutletParSlipVelocityFvPatchVectorField::
pressureInletOutletParSlipVelocityFvPatchVectorField
(
	const fvPatch& p,
	const DimensionedField<vector, volMesh>& iF
)
	:
	mixedFvPatchVectorField(p, iF),
	phiName_("phi"),
	rhoName_("rho")
{
	refValue() = *this;
	refGrad() = Zero;
	valueFraction() = 0.0;
}


tnbLib::pressureInletOutletParSlipVelocityFvPatchVectorField::
pressureInletOutletParSlipVelocityFvPatchVectorField
(
	const pressureInletOutletParSlipVelocityFvPatchVectorField& ptf,
	const fvPatch& p,
	const DimensionedField<vector, volMesh>& iF,
	const fvPatchFieldMapper& mapper
)
	:
	mixedFvPatchVectorField(ptf, p, iF, mapper),
	phiName_(ptf.phiName_),
	rhoName_(ptf.rhoName_)
{}


tnbLib::pressureInletOutletParSlipVelocityFvPatchVectorField::
pressureInletOutletParSlipVelocityFvPatchVectorField
(
	const fvPatch& p,
	const DimensionedField<vector, volMesh>& iF,
	const dictionary& dict
)
	:
	mixedFvPatchVectorField(p, iF),
	phiName_(dict.lookupOrDefault<word>("phi", "phi")),
	rhoName_(dict.lookupOrDefault<word>("rho", "rho"))
{
	fvPatchVectorField::operator=(vectorField("value", dict, p.size()));
	refValue() = *this;
	refGrad() = Zero;
	valueFraction() = 0.0;
}


tnbLib::pressureInletOutletParSlipVelocityFvPatchVectorField::
pressureInletOutletParSlipVelocityFvPatchVectorField
(
	const pressureInletOutletParSlipVelocityFvPatchVectorField& pivpvf
)
	:
	mixedFvPatchVectorField(pivpvf),
	phiName_(pivpvf.phiName_),
	rhoName_(pivpvf.rhoName_)
{}


tnbLib::pressureInletOutletParSlipVelocityFvPatchVectorField::
pressureInletOutletParSlipVelocityFvPatchVectorField
(
	const pressureInletOutletParSlipVelocityFvPatchVectorField& pivpvf,
	const DimensionedField<vector, volMesh>& iF
)
	:
	mixedFvPatchVectorField(pivpvf, iF),
	phiName_(pivpvf.phiName_),
	rhoName_(pivpvf.rhoName_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void tnbLib::pressureInletOutletParSlipVelocityFvPatchVectorField::updateCoeffs()
{
	if (updated())
	{
		return;
	}

	const surfaceScalarField& phi =
		db().lookupObject<surfaceScalarField>(phiName_);

	const fvsPatchField<scalar>& phip =
		patch().patchField<surfaceScalarField, scalar>(phi);

	tmp<vectorField> n = patch().nf();
	const Field<scalar>& magSf = patch().magSf();

	// Get the tangential component from the internalField (zero-gradient)
	vectorField Ut(patchInternalField());
	Ut -= n()*(Ut & n());

	if (phi.dimensions() == dimVelocity * dimArea)
	{
		refValue() = Ut + n * phip / magSf;
	}
	else if (phi.dimensions() == dimDensity * dimVelocity*dimArea)
	{
		const fvPatchField<scalar>& rhop =
			patch().lookupPatchField<volScalarField, scalar>(rhoName_);

		refValue() = Ut + n * phip / (rhop*magSf);
	}
	else
	{
		FatalErrorInFunction
			<< "dimensions of phi are not correct" << nl
			<< "    on patch " << this->patch().name()
			<< " of field " << this->internalField().name()
			<< " in file " << this->internalField().objectPath()
			<< exit(FatalError);
	}

	valueFraction() = 1.0 - pos0(phip);

	mixedFvPatchVectorField::updateCoeffs();
}


void tnbLib::pressureInletOutletParSlipVelocityFvPatchVectorField::write
(
	Ostream& os
) const
{
	fvPatchVectorField::write(os);
	writeEntryIfDifferent<word>(os, "phi", "phi", phiName_);
	writeEntryIfDifferent<word>(os, "rho", "rho", rhoName_);
	writeEntry(os, "value", *this);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

void tnbLib::pressureInletOutletParSlipVelocityFvPatchVectorField::operator=
(
	const fvPatchField<vector>& pvf
	)
{
	fvPatchField<vector>::operator=(pvf);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	makePatchTypeField
	(
		fvPatchVectorField,
		pressureInletOutletParSlipVelocityFvPatchVectorField
	);
}

// ************************************************************************* //