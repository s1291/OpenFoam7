#pragma once
#ifndef _directions_Header
#define _directions_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2019 OpenFOAM Foundation
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
    tnbLib::directions

Description
    Set of directions for each cell in the mesh. Either uniform and size=1
    or one set of directions per cell.

    Used in splitting cells.
    Either all cells have similar refinement direction ('global') or
    direction is dependent on local cell geometry, or loads selected fields
    by name ('fieldBased'). Controlled by dictionary.

SourceFiles
    directions.C

\*---------------------------------------------------------------------------*/

#include <List.hxx>
#include <vectorField.hxx>
#include <NamedEnum.hxx>
#include <point.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

    // Forward declaration of classes
    class polyMesh;
    class twoDPointCorrector;
    class primitiveMesh;
    class polyPatch;
    class dictionary;

    /*---------------------------------------------------------------------------*\
                               Class directions Declaration
    \*---------------------------------------------------------------------------*/

    class directions
        :
        public List<vectorField>
    {

    public:

        // Data types

            //- Enumeration listing the possible coordinate directions.
        enum directionType
        {
            TAN1,
            TAN2,
            NORMAL
        };


    private:

        static FoamDynamicMesh_EXPORT const NamedEnum<directionType, 3> directionTypeNames_;


        // Private Member Functions

            //- For debugging. Write point coordinate.
        static FoamDynamicMesh_EXPORT void writeOBJ(Ostream& os, const point& pt);

        //- For debugging. Write edge between two points.
        static FoamDynamicMesh_EXPORT void writeOBJ
        (
            Ostream& os,
            const point& pt0,
            const point& pt1,
            label& vertI
        );

        //- For debugging. Write hedgehog display of vectorField as obj file.
        static FoamDynamicMesh_EXPORT void writeOBJ
        (
            const fileName& fName,
            const primitiveMesh& mesh,
            const vectorField& dirs
        );

        //- Check if vec has no component in 2D normal direction. Exits if
        //  so.
        static FoamDynamicMesh_EXPORT void check2D
        (
            const twoDPointCorrector* correct2DPtr,
            const vector& vec
        );

        //- Get coordinate direction for all cells in mesh by propagating from
        //  vector on patch.
        static FoamDynamicMesh_EXPORT vectorField propagateDirection
        (
            const polyMesh& mesh,
            const bool useTopo,
            const polyPatch& pp,
            const vectorField& ppField,
            const vector& defaultDir
        );


    public:

        // Constructors

            //- Construct from mesh and dictionary and optional 2D corrector.
        FoamDynamicMesh_EXPORT directions
        (
            const polyMesh& mesh,
            const dictionary& dict,
            const twoDPointCorrector* correct2DPtr = nullptr
        );

        //- Disallow default bitwise copy construction
        directions(const directions&) = delete;


        // Member Operators

            //- Disallow default bitwise assignment
        void operator=(const directions&) = delete;
    };


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_directions_Header
