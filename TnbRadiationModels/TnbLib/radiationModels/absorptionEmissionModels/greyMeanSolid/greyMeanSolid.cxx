#include <greyMeanSolid.hxx>

#include <addToRunTimeSelectionTable.hxx>
#include <unitConversion.hxx>
#include <extrapolatedCalculatedFvPatchFields.hxx>

#include <Time.hxx>  // added by amir

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace radiationModels
	{
		namespace absorptionEmissionModels
		{
			defineTypeNameAndDebug(greyMeanSolid, 0);

			addToRunTimeSelectionTable
			(
				absorptionEmissionModel,
				greyMeanSolid,
				dictionary
			);
		}
	}
}


// * * * * * * * * * * * * * * Private Member Functions * * * * * * * * * * //

tnbLib::tmp<tnbLib::scalarField>
tnbLib::radiationModels::absorptionEmissionModels::greyMeanSolid::X
(
	const word specie
) const
{
	const volScalarField& T = thermo_.T();
	const volScalarField& p = thermo_.p();

	tmp<scalarField> tXj(new scalarField(T.primitiveField().size(), 0.0));
	scalarField& Xj = tXj.ref();

	tmp<scalarField> tRhoInv(new scalarField(T.primitiveField().size(), 0.0));
	scalarField& rhoInv = tRhoInv.ref();

	forAll(mixture_.Y(), specieI)
	{
		const scalarField& Yi = mixture_.Y()[specieI];

		forAll(rhoInv, iCell)
		{
			rhoInv[iCell] +=
				Yi[iCell] / mixture_.rho(specieI, p[iCell], T[iCell]);
		}
	}
	const scalarField& Yj = mixture_.Y(specie);
	const label mySpecieI = mixture_.species()[specie];
	forAll(Xj, iCell)
	{
		Xj[iCell] = Yj[iCell] / mixture_.rho(mySpecieI, p[iCell], T[iCell]);
	}

	return (Xj / rhoInv);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tnbLib::radiationModels::absorptionEmissionModels::greyMeanSolid::greyMeanSolid
(
	const dictionary& dict,
	const fvMesh& mesh
)
	:
	absorptionEmissionModel(dict, mesh),
	coeffsDict_((dict.optionalSubDict(typeName + "Coeffs"))),
	thermo_(mesh.lookupObject<solidThermo>(basicThermo::dictName)),
	speciesNames_(0),
	mixture_(dynamic_cast<const basicSpecieMixture&>(thermo_)),
	solidData_(mixture_.Y().size())
{
	if (!isA<basicSpecieMixture>(thermo_))
	{
		FatalErrorInFunction
			<< "Model requires a multi-component thermo package"
			<< abort(FatalError);
	}

	label nFunc = 0;
	const dictionary& functionDicts = dict.optionalSubDict(typeName + "Coeffs");

	forAllConstIter(dictionary, functionDicts, iter)
	{
		// safety:
		if (!iter().isDict())
		{
			continue;
		}
		const word& key = iter().keyword();
		if (!mixture_.contains(key))
		{
			WarningInFunction
				<< " specie: " << key << " is not found in the solid mixture"
				<< nl
				<< " specie is the mixture are:" << mixture_.species() << nl
				<< nl << endl;
		}
		speciesNames_.insert(key, nFunc);
		const dictionary& dict = iter().dict();
		dict.lookup("absorptivity") >> solidData_[nFunc][absorptivity];
		dict.lookup("emissivity") >> solidData_[nFunc][emissivity];

		nFunc++;
	}
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

tnbLib::radiationModels::absorptionEmissionModels::greyMeanSolid::~greyMeanSolid()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

tnbLib::tmp<tnbLib::volScalarField>
tnbLib::radiationModels::absorptionEmissionModels::greyMeanSolid::calc
(
	const label propertyId
) const
{
	tmp<volScalarField> ta
	(
		new volScalarField
		(
			IOobject
			(
				"a",
				mesh().time().timeName(),
				mesh(),
				IOobject::NO_READ,
				IOobject::NO_WRITE
			),
			mesh(),
			dimensionedScalar(dimless / dimLength, 0),
			extrapolatedCalculatedFvPatchVectorField::typeName
		)
	);

	scalarField& a = ta.ref().primitiveFieldRef();

	forAllConstIter(HashTable<label>, speciesNames_, iter)
	{
		if (mixture_.contains(iter.key()))
		{
			a += solidData_[iter()][propertyId] * X(iter.key());
		}
	}

	ta.ref().correctBoundaryConditions();
	return ta;
}


tnbLib::tmp<tnbLib::volScalarField>
tnbLib::radiationModels::absorptionEmissionModels::greyMeanSolid::eCont
(
	const label bandI
) const
{
	return calc(emissivity);
}


tnbLib::tmp<tnbLib::volScalarField>
tnbLib::radiationModels::absorptionEmissionModels::greyMeanSolid::aCont
(
	const label bandI
) const
{
	return calc(absorptivity);
}

// ************************************************************************* //