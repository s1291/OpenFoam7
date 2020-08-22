#include <componentDisplacementMotionSolver.hxx>

#include <mapPolyMesh.hxx>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace tnbLib
{
	defineTypeNameAndDebug(componentDisplacementMotionSolver, 0);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

tnbLib::direction tnbLib::componentDisplacementMotionSolver::cmpt
(
	const word& cmptName
) const
{
	if (cmptName == "x")
	{
		return vector::X;
	}
	else if (cmptName == "y")
	{
		return vector::Y;
	}
	else if (cmptName == "z")
	{
		return vector::Z;
	}
	else
	{
		FatalErrorInFunction
			<< "Given component name " << cmptName << " should be x, y or z"
			<< exit(FatalError);

		return 0;
	}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tnbLib::componentDisplacementMotionSolver::componentDisplacementMotionSolver
(
	const polyMesh& mesh,
	const dictionary& dict,
	const word& type
)
	:
	motionSolver(mesh, dict, type),
	cmptName_(coeffDict().lookup("component")),
	cmpt_(cmpt(cmptName_)),
	points0_
	(
		pointIOField
		(
			IOobject
			(
				"points",
				mesh.time().constant(),
				polyMesh::meshSubDir,
				mesh,
				IOobject::MUST_READ,
				IOobject::NO_WRITE,
				false
			)
		).component(cmpt_)
	),
	pointDisplacement_
	(
		IOobject
		(
			"pointDisplacement" + cmptName_,
			mesh.time().timeName(),
			mesh,
			IOobject::MUST_READ,
			IOobject::AUTO_WRITE
		),
		pointMesh::New(mesh)
	)
{
	if (points0_.size() != mesh.nPoints())
	{
		FatalErrorInFunction
			<< "Number of points in mesh " << mesh.nPoints()
			<< " differs from number of points " << points0_.size()
			<< " read from file "
			<< typeFilePath<pointIOField>
			(
				IOobject
				(
					"points",
					mesh.time().constant(),
					polyMesh::meshSubDir,
					mesh,
					IOobject::MUST_READ,
					IOobject::NO_WRITE,
					false
				)
				)
			<< exit(FatalError);
	}
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

tnbLib::componentDisplacementMotionSolver::~componentDisplacementMotionSolver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void tnbLib::componentDisplacementMotionSolver::movePoints(const pointField& p)
{
	// No local data to update
}


void tnbLib::componentDisplacementMotionSolver::updateMesh(const mapPolyMesh& mpm)
{
	// pointMesh already updates pointFields.

	motionSolver::updateMesh(mpm);

	// Map points0_. Bit special since we somehow have to come up with
	// a sensible points0 position for introduced points.
	// Find out scaling between points0 and current points

	// Get the new points either from the map or the mesh
	const scalarField points
	(
		mpm.hasMotionPoints()
		? mpm.preMotionPoints().component(cmpt_)
		: mesh().points().component(cmpt_)
	);

	// Get extents of points0 and points and determine scale
	const scalar scale =
		(gMax(points0_) - gMin(points0_))
		/ (gMax(points) - gMin(points));

	scalarField newPoints0(mpm.pointMap().size());

	forAll(newPoints0, pointi)
	{
		label oldPointi = mpm.pointMap()[pointi];

		if (oldPointi >= 0)
		{
			label masterPointi = mpm.reversePointMap()[oldPointi];

			if (masterPointi == pointi)
			{
				newPoints0[pointi] = points0_[oldPointi];
			}
			else
			{
				// New point. Assume motion is scaling.
				newPoints0[pointi] =
					points0_[oldPointi]
					+ scale * (points[pointi] - points[masterPointi]);
			}
		}
		else
		{
			FatalErrorInFunction
				<< "Cannot work out coordinates of introduced vertices."
				<< " New vertex " << pointi << " at coordinate "
				<< points[pointi] << exit(FatalError);
		}
	}
	points0_.transfer(newPoints0);
}


// ************************************************************************* //