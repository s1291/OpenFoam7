/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2012-2018 OpenFOAM Foundation
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

#include <fieldFromFile.hxx>

#include <addToRunTimeSelectionTable.hxx>
#include <triSurfaceMesh.hxx>
#include <triSurfaceFields.hxx>
#include <Time.hxx>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace tnbLib
{
    defineTypeNameAndDebug(fieldFromFile, 0);
    addToRunTimeSelectionTable
    (
        cellSizeCalculationType,
        fieldFromFile,
        dictionary
    );
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tnbLib::fieldFromFile::fieldFromFile
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
    fileName_
    (
        cellSizeCalcTypeDict.optionalSubDict
        (
            typeName + "Coeffs"
        ).lookup("fieldFile")
    ),
    cellSizeMultipleCoeff_
    (
        coeffsDict_.lookupOrDefault<scalar>("cellSizeMultipleCoeff", 1)
    )
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

tnbLib::tmp<tnbLib::triSurfacePointScalarField> tnbLib::fieldFromFile::load()
{
    Info << indent << "Loading: " << fileName_ << endl;

    tmp<triSurfacePointScalarField> pointCellSize
    (
        new triSurfacePointScalarField
        (
            IOobject
            (
                fileName_,
                surface_.searchableSurface::time().constant(),
                "triSurface",
                surface_.searchableSurface::time(),
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            ),
            surface_,
            dimLength,
            true
        )
    );

    pointCellSize.ref() *= cellSizeMultipleCoeff_;

    return pointCellSize;
}


// ************************************************************************* //
