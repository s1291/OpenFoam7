/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2012-2019 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include <automatic.hxx>

#include <addToRunTimeSelectionTable.hxx>
#include <triSurfaceMesh.hxx>
#include <vtkSurfaceWriter.hxx>
#include <primitivePatchInterpolation.hxx>
#include <Time.hxx>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace tnbLib
{
    defineTypeNameAndDebug(automatic, 0);
    addToRunTimeSelectionTable(cellSizeCalculationType, automatic, dictionary);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void tnbLib::automatic::smoothField(triSurfaceScalarField& surf)
{
    label nSmoothingIterations = 10;

    for (label iter = 0; iter < nSmoothingIterations; ++iter)
    {
        const pointField& faceCentres = surface_.faceCentres();

        forAll(surf, sI)
        {
            const labelList& faceFaces = surface_.faceFaces()[sI];

            const point& fC = faceCentres[sI];
            const scalar value = surf[sI];

            scalar newValue = 0;
            scalar totalDist = 0;

            label nFaces = 0;

            forAll(faceFaces, fI)
            {
                const label faceLabel = faceFaces[fI];
                const point& faceCentre = faceCentres[faceLabel];

                const scalar faceValue = surf[faceLabel];
                const scalar distance = mag(faceCentre - fC);

                newValue += faceValue / (distance + small);

                totalDist += 1.0 / (distance + small);

                if (value < faceValue)
                {
                    nFaces++;
                }
            }

            // Do not smooth out the peak values
            if (nFaces == faceFaces.size())
            {
                continue;
            }

            surf[sI] = newValue / totalDist;
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tnbLib::automatic::automatic
(
    const dictionary& cellSizeCalcTypeDict,
    const triSurfaceMesh& surface,
    const scalar& defaultCellSize
)
    :
    cellSizeCalculationType
    (
        typeName,
        cellSizeCalcTypeDict,
        surface,
        defaultCellSize
    ),
    coeffsDict_(cellSizeCalcTypeDict.optionalSubDict(typeName + "Coeffs")),
    surfaceName_(surface.searchableSurface::name()),
    readCurvature_(Switch(coeffsDict_.lookup("curvature"))),
    curvatureFile_(coeffsDict_.lookup("curvatureFile")),
    readFeatureProximity_(Switch(coeffsDict_.lookup("featureProximity"))),
    featureProximityFile_(coeffsDict_.lookup("featureProximityFile")),
    readInternalCloseness_(Switch(coeffsDict_.lookup("internalCloseness"))),
    internalClosenessFile_(coeffsDict_.lookup("internalClosenessFile")),
    internalClosenessCellSizeCoeff_
    (
        readScalar(coeffsDict_.lookup("internalClosenessCellSizeCoeff"))
    ),
    curvatureCellSizeCoeff_
    (
        readScalar(coeffsDict_.lookup("curvatureCellSizeCoeff"))
    ),
    maximumCellSize_
    (
        readScalar(coeffsDict_.lookup("maximumCellSizeCoeff"))* defaultCellSize
    )
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

tnbLib::tmp<tnbLib::triSurfacePointScalarField> tnbLib::automatic::load()
{
    Info << indent
        << "Calculating cell size on surface: " << surfaceName_ << endl;

    tmp<triSurfacePointScalarField> tPointCellSize
    (
        new triSurfacePointScalarField
        (
            IOobject
            (
                surfaceName_ + ".cellSize",
                surface_.searchableSurface::time().constant(),
                "triSurface",
                surface_.searchableSurface::time(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            surface_,
            dimLength,
            scalarField(surface_.nPoints(), maximumCellSize_)
        )
    );

    triSurfacePointScalarField& pointCellSize = tPointCellSize.ref();

    if (readCurvature_)
    {
        Info << indent
            << "Reading curvature         : " << curvatureFile_ << endl;

        triSurfacePointScalarField curvature
        (
            IOobject
            (
                curvatureFile_,
                surface_.searchableSurface::time().constant(),
                "triSurface",
                surface_.searchableSurface::time(),
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            ),
            surface_,
            dimLength,
            true
        );

        forAll(pointCellSize, pI)
        {
            pointCellSize[pI] =
                min
                (
                    1.0
                    / max
                    (
                        (1.0 / curvatureCellSizeCoeff_) * mag(curvature[pI]),
                        1.0 / maximumCellSize_
                    ),
                    pointCellSize[pI]
                );
        }
    }

    PrimitivePatchInterpolation
        <
        PrimitivePatch<::tnbLib::List<labelledTri>, pointField>
        >
        patchInterpolate(surface_);

    const Map<label>& meshPointMap = surface_.meshPointMap();

    if (readInternalCloseness_)
    {
        Info << indent
            << "Reading internal closeness: " << internalClosenessFile_ << endl;

        triSurfacePointScalarField internalClosenessPointField
        (
            IOobject
            (
                internalClosenessFile_,
                surface_.searchableSurface::time().constant(),
                "triSurface",
                surface_.searchableSurface::time(),
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            ),
            surface_,
            dimLength,
            true
        );

        forAll(pointCellSize, pI)
        {
            pointCellSize[pI] =
                min
                (
                    (1.0 / internalClosenessCellSizeCoeff_)
                    * internalClosenessPointField[meshPointMap[pI]],
                    pointCellSize[pI]
                );
        }
    }

    if (readFeatureProximity_)
    {
        Info << indent
            << "Reading feature proximity : " << featureProximityFile_ << endl;

        triSurfaceScalarField featureProximity
        (
            IOobject
            (
                featureProximityFile_,
                surface_.searchableSurface::time().constant(),
                "triSurface",
                surface_.searchableSurface::time(),
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            ),
            surface_,
            dimLength,
            true
        );

        scalarField featureProximityPointField
        (
            patchInterpolate.faceToPointInterpolate(featureProximity)
        );

        forAll(pointCellSize, pI)
        {
            pointCellSize[pI] =
                min
                (
                    featureProximityPointField[meshPointMap[pI]],
                    pointCellSize[pI]
                );
        }
    }

    // smoothField(surfaceCellSize);

    pointCellSize.write();

    if (debug)
    {
        faceList faces(surface_.size());

        forAll(surface_, fI)
        {
            faces[fI] = surface_.triSurface::operator[](fI).triFaceFace();
        }

        vtkSurfaceWriter().write
        (
            surface_.searchableSurface::time().constant() / "triSurface",
            surfaceName_.lessExt().name(),
            surface_.points(),
            faces,
            "cellSize",
            pointCellSize,
            true,
            true
        );
    }

    return tPointCellSize;
}


// ************************************************************************* //
