#include <sampledPatchInternalField.hxx>

#include <interpolationCellPoint.hxx>
#include <PrimitivePatchInterpolationTemplate.hxx>

#include <indirectPrimitivePatch.hxx>  // added by amir

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Type>
tnbLib::tmp<tnbLib::Field<Type>>
tnbLib::sampledSurfaces::patchInternalField::sampleField
(
	const GeometricField<Type, fvPatchField, volMesh>& vField
) const
{
	// One value per face
	tmp<Field<Type>> tvalues(new Field<Type>(patchFaceLabels().size()));
	Field<Type>& values = tvalues.ref();

	forAll(patchStart(), i)
	{
		// Get patchface wise data by sampling internal field
		Field<Type> interpVals = vField.primitiveField();
		mappers_[i].map().distribute(interpVals);

		// Store at correct position in values
		label end =
			(
				i < patchStart().size() - 1
				? patchStart()[i + 1]
				: patchFaceLabels().size()
				);

		for (label triI = patchStart()[i]; triI < end; triI++)
		{
			values[triI] = interpVals[patchFaceLabels()[triI]];
		}
	}

	return tvalues;
}


template<class Type>
tnbLib::tmp<tnbLib::Field<Type>>
tnbLib::sampledSurfaces::patchInternalField::interpolateField
(
	const interpolation<Type>& interpolator
) const
{
	label sz = 0;
	forAll(patchIDs(), i)
	{
		sz += mesh().boundaryMesh()[patchIDs()[i]].size();
	}

	Field<Type> allPatchVals(sz);
	sz = 0;

	forAll(patchIDs(), i)
	{
		// See mappedFixedValueFvPatchField
		const mapDistribute& distMap = mappers_[i].map();

		// Send back sample points to processor that holds the cell.
		// Mark cells with point::max so we know which ones we need
		// to interpolate (since expensive).
		vectorField samples(mappers_[i].samplePoints());
		distMap.reverseDistribute(mesh().nCells(), point::max, samples);

		Field<Type> patchVals(mesh().nCells());

		forAll(samples, celli)
		{
			if (samples[celli] != point::max)
			{
				patchVals[celli] = interpolator.interpolate
				(
					samples[celli],
					celli
				);
			}
		}

		distMap.distribute(patchVals);

		// Now patchVals holds the interpolated data in patch face order.
		// Collect.
		SubList<Type>(allPatchVals, patchVals.size(), sz) = patchVals;
		sz += patchVals.size();
	}

	// Interpolate to points. Reconstruct the patch of all faces to aid
	// interpolation.

	labelList meshFaceLabels(allPatchVals.size());
	sz = 0;
	forAll(patchIDs(), i)
	{
		const polyPatch& pp = mesh().boundaryMesh()[patchIDs()[i]];
		forAll(pp, i)
		{
			meshFaceLabels[sz++] = pp.start() + i;
		}
	}

	indirectPrimitivePatch allPatches
	(
		IndirectList<face>(mesh().faces(), meshFaceLabels),
		mesh().points()
	);

	return PrimitivePatchInterpolation<indirectPrimitivePatch>
		(
			allPatches
			).faceToPointInterpolate(allPatchVals);
}


// ************************************************************************* //