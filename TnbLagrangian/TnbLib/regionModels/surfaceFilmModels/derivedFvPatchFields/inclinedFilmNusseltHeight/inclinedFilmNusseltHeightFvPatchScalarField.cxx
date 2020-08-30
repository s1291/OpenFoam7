#include <inclinedFilmNusseltHeightFvPatchScalarField.hxx>

#include <volFields.hxx>
#include <kinematicSingleLayer.hxx>
#include <addToRunTimeSelectionTable.hxx>

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tnbLib::inclinedFilmNusseltHeightFvPatchScalarField::
inclinedFilmNusseltHeightFvPatchScalarField
(
	const fvPatch& p,
	const DimensionedField<scalar, volMesh>& iF
)
	:
	fixedValueFvPatchScalarField(p, iF),
	GammaMean_(),
	a_(),
	omega_()
{}


tnbLib::inclinedFilmNusseltHeightFvPatchScalarField::
inclinedFilmNusseltHeightFvPatchScalarField
(
	const inclinedFilmNusseltHeightFvPatchScalarField& ptf,
	const fvPatch& p,
	const DimensionedField<scalar, volMesh>& iF,
	const fvPatchFieldMapper& mapper
)
	:
	fixedValueFvPatchScalarField(ptf, p, iF, mapper),
	GammaMean_(ptf.GammaMean_().clone().ptr()),
	a_(ptf.a_().clone().ptr()),
	omega_(ptf.omega_().clone().ptr())
{}


tnbLib::inclinedFilmNusseltHeightFvPatchScalarField::
inclinedFilmNusseltHeightFvPatchScalarField
(
	const fvPatch& p,
	const DimensionedField<scalar, volMesh>& iF,
	const dictionary& dict
)
	:
	fixedValueFvPatchScalarField(p, iF, dict),
	GammaMean_(Function1<scalar>::New("GammaMean", dict)),
	a_(Function1<scalar>::New("a", dict)),
	omega_(Function1<scalar>::New("omega", dict))
{}


tnbLib::inclinedFilmNusseltHeightFvPatchScalarField::
inclinedFilmNusseltHeightFvPatchScalarField
(
	const inclinedFilmNusseltHeightFvPatchScalarField& wmfrhpsf
)
	:
	fixedValueFvPatchScalarField(wmfrhpsf),
	GammaMean_(wmfrhpsf.GammaMean_().clone().ptr()),
	a_(wmfrhpsf.a_().clone().ptr()),
	omega_(wmfrhpsf.omega_().clone().ptr())
{}


tnbLib::inclinedFilmNusseltHeightFvPatchScalarField::
inclinedFilmNusseltHeightFvPatchScalarField
(
	const inclinedFilmNusseltHeightFvPatchScalarField& wmfrhpsf,
	const DimensionedField<scalar, volMesh>& iF
)
	:
	fixedValueFvPatchScalarField(wmfrhpsf, iF),
	GammaMean_(wmfrhpsf.GammaMean_().clone().ptr()),
	a_(wmfrhpsf.a_().clone().ptr()),
	omega_(wmfrhpsf.omega_().clone().ptr())
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void tnbLib::inclinedFilmNusseltHeightFvPatchScalarField::updateCoeffs()
{
	if (updated())
	{
		return;
	}

	const label patchi = patch().index();

	// retrieve the film region from the database

	const regionModels::regionModel& region =
		db().time().lookupObject<regionModels::regionModel>
		(
			"surfaceFilmProperties"
			);

	const regionModels::surfaceFilmModels::kinematicSingleLayer& film =
		dynamic_cast
		<
		const regionModels::surfaceFilmModels::kinematicSingleLayer&
		>(region);

	// calculate the vector tangential to the patch

	// note: normal pointing into the domain
	const vectorField n(-patch().nf());

	// TODO: currently re-evaluating the entire gTan field to return this patch
	const scalarField gTan(film.gTan()().boundaryField()[patchi] & n);

	if (patch().size() && (max(mag(gTan)) < small))
	{
		WarningInFunction
			<< "is designed to operate on patches inclined with respect to "
			<< "gravity"
			<< endl;
	}

	const volVectorField& nHat = film.nHat();

	const vectorField nHatp(nHat.boundaryField()[patchi].patchInternalField());

	vectorField nTan(nHatp ^ n);
	nTan /= mag(nTan) + rootVSmall;

	// calculate distance in patch tangential direction

	const vectorField& Cf = patch().Cf();
	scalarField d(nTan & Cf);

	// calculate the wavy film height

	const scalar t = db().time().timeOutputValue();

	const scalar GMean = GammaMean_->value(t);
	const scalar a = a_->value(t);
	const scalar omega = omega_->value(t);

	const scalarField G(GMean + a * sin(omega*constant::mathematical::twoPi*d));

	const volScalarField& mu = film.mu();
	const scalarField mup(mu.boundaryField()[patchi].patchInternalField());

	const volScalarField& rho = film.rho();
	const scalarField rhop(rho.boundaryField()[patchi].patchInternalField());

	const scalarField Re(max(G, scalar(0)) / mup);

	operator==
		(
			pow(3 * sqr(mup / rhop) / (gTan + rootVSmall), 1.0 / 3.0)*pow(Re, 1.0 / 3.0)
			);

	fixedValueFvPatchScalarField::updateCoeffs();
}


void tnbLib::inclinedFilmNusseltHeightFvPatchScalarField::write
(
	Ostream& os
) const
{
	fixedValueFvPatchScalarField::write(os);
	writeEntry(os, GammaMean_());
	writeEntry(os, a_());
	writeEntry(os, omega_());
	writeEntry(os, "value", *this);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	makePatchTypeField
	(
		fvPatchScalarField,
		inclinedFilmNusseltHeightFvPatchScalarField
	);
}


// ************************************************************************* //