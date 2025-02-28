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

#include <cellSizeFunction.hxx>

#include <addToRunTimeSelectionTable.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
    defineTypeNameAndDebug(cellSizeFunction, 0);
    defineRunTimeSelectionTable(cellSizeFunction, dictionary);

    scalar cellSizeFunction::snapToSurfaceTol_ = 1e-10;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tnbLib::cellSizeFunction::cellSizeFunction
(
    const word& type,
    const dictionary& cellSizeFunctionDict,
    const searchableSurface& surface,
    const scalar& defaultCellSize,
    const labelList regionIndices
)
    :
    dictionary(cellSizeFunctionDict),
    surface_(surface),
    surfaceCellSizeFunction_
    (
        surfaceCellSizeFunction::New
        (
            cellSizeFunctionDict,
            surface,
            defaultCellSize
        )
    ),
    coeffsDict_(optionalSubDict(type + "Coeffs")),
    defaultCellSize_(defaultCellSize),
    regionIndices_(regionIndices),
    sideMode_(),
    priority_(readLabel(cellSizeFunctionDict.lookup("priority", true)))
{
    word mode = cellSizeFunctionDict.lookup("mode", true);

    if (surface_.hasVolumeType())
    {
        if (mode == "inside")
        {
            sideMode_ = smInside;
        }
        else if (mode == "outside")
        {
            sideMode_ = smOutside;
        }
        else if (mode == "bothSides")
        {
            sideMode_ = rmBothsides;
        }
        else
        {
            FatalErrorInFunction
                << "Unknown mode, expected: inside, outside or bothSides" << nl
                << exit(FatalError);
        }
    }
    else
    {
        if (mode != "bothSides")
        {
            WarningInFunction
                << "surface does not support volumeType, defaulting mode to "
                << "bothSides."
                << endl;
        }

        sideMode_ = rmBothsides;
    }

    if (debug)
    {
        Info << nl
            << "Cell size function for surface " << surface.name()
            << ", " << mode
            << ", priority = " << priority_
            << ", regions = " << regionIndices_
            << endl;
    }
}


// * * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * //

tnbLib::autoPtr<tnbLib::cellSizeFunction> tnbLib::cellSizeFunction::New
(
    const dictionary& cellSizeFunctionDict,
    const searchableSurface& surface,
    const scalar& defaultCellSize,
    const labelList regionIndices
)
{
    word cellSizeFunctionTypeName
    (
        cellSizeFunctionDict.lookup("cellSizeFunction")
    );

    Info << indent << "Selecting cellSizeFunction " << cellSizeFunctionTypeName
        << endl;

    dictionaryConstructorTable::iterator cstrIter =
        dictionaryConstructorTablePtr_->find(cellSizeFunctionTypeName);

    if (cstrIter == dictionaryConstructorTablePtr_->end())
    {
        FatalErrorInFunction
            << "Unknown cellSizeFunction type "
            << cellSizeFunctionTypeName
            << endl << endl
            << "Valid cellSizeFunction types are :" << endl
            << dictionaryConstructorTablePtr_->toc()
            << exit(FatalError);
    }

    return autoPtr<cellSizeFunction>
        (
            cstrIter()
            (
                cellSizeFunctionDict,
                surface,
                defaultCellSize,
                regionIndices
                )
            );
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

tnbLib::cellSizeFunction::~cellSizeFunction()
{}


// ************************************************************************* //
