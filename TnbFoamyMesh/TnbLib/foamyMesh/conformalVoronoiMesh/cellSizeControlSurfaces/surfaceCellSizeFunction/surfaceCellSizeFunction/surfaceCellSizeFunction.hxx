#pragma once
#ifndef _surfaceCellSizeFunction_Header
#define _surfaceCellSizeFunction_Header

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

Class
    tnbLib::surfaceCellSizeFunction

Description
    Abstract base class for specifying target cell sizes

SourceFiles
    surfaceCellSizeFunction.C

\*---------------------------------------------------------------------------*/

#include <FoamyMesh_Module.hxx>

#include <searchableSurface.hxx>
#include <dictionary.hxx>
#include <autoPtr.hxx>
#include <runTimeSelectionTables.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

    /*---------------------------------------------------------------------------*\
                         Class surfaceCellSizeFunction Declaration
    \*---------------------------------------------------------------------------*/

    class surfaceCellSizeFunction
        :
        public dictionary
    {

    protected:

        // Protected data

            //- Reference to the searchableSurface that surfaceCellSizeFunction
            //  relates to
        const searchableSurface& surface_;

        const dictionary coeffsDict_;

        const scalar& defaultCellSize_;

        //- If cell resizing is allowed, this is the factor of the old cell size
        //  to get the new cell size
        scalar refinementFactor_;


    public:

        //- Runtime type information
        TypeName("surfaceCellSizeFunction");


        // Declare run-time constructor selection table

        declareRunTimeSelectionTable
        (
            autoPtr,
            surfaceCellSizeFunction,
            dictionary,
            (
                const dictionary& surfaceCellSizeFunctionDict,
                const searchableSurface& surface,
                const scalar& defaultCellSize
                ),
            (surfaceCellSizeFunctionDict, surface, defaultCellSize)
        );


        // Constructors

            //- Construct from components
        surfaceCellSizeFunction
        (
            const word& type,
            const dictionary& surfaceCellSizeFunctionDict,
            const searchableSurface& surface,
            const scalar& defaultCellSize
        );

        //- Disallow default bitwise copy construction
        surfaceCellSizeFunction(const surfaceCellSizeFunction&) = delete;


        // Selectors

            //- Return a reference to the selected surfaceCellSizeFunction
        static autoPtr<surfaceCellSizeFunction> New
        (
            const dictionary& surfaceCellSizeFunctionDict,
            const searchableSurface& surface,
            const scalar& defaultCellSize
        );


        //- Destructor
        virtual ~surfaceCellSizeFunction();


        // Member Functions

            //- Const access to the details dictionary
        inline const dictionary& coeffsDict() const
        {
            return coeffsDict_;
        }

        virtual scalar interpolate
        (
            const point& pt,
            const label index
        ) const = 0;


        // Member Operators

            //- Disallow default bitwise assignment
        void operator=(const surfaceCellSizeFunction&) = delete;
    };


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#endif // !_surfaceCellSizeFunction_Header
