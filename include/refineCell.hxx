#pragma once
#ifndef _refineCell_Header
#define _refineCell_Header

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
    tnbLib::refineCell

Description
    Container with cells to refine. Refinement given as single direction.

SourceFiles
    refineCell.C

\*---------------------------------------------------------------------------*/

#include <label.hxx>
#include <vector.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

    // Forward declaration of friend functions and operators

    class refineCell;

    FoamDynamicMesh_EXPORT Ostream& operator<<(Ostream&, const refineCell&);


    /*---------------------------------------------------------------------------*\
                               Class refineCell Declaration
    \*---------------------------------------------------------------------------*/

    class refineCell
    {
        // Private Data

        //- Cell label
        label cellNo_;

        //- Preferred refinement direction (always normalized).
        vector direction_;

    public:

        // Constructors

            //- Null
        FoamDynamicMesh_EXPORT refineCell();

        //- From components. Vector will be normalized upon construction.
        FoamDynamicMesh_EXPORT refineCell(const label, const vector&);

        //- From Istream. Vector will be normalized upon construction.
        FoamDynamicMesh_EXPORT refineCell(Istream& is);


        // Member Functions

        label cellNo() const
        {
            return cellNo_;
        }

        const vector& direction() const
        {
            return direction_;
        }


        // Friend Operators

        inline friend bool operator==
            (
                const refineCell& rc1,
                const refineCell& rc2
                )
        {
            return (rc1.cellNo() == rc2.cellNo());
        }

        inline friend bool operator!=
            (
                const refineCell& rc1,
                const refineCell& rc2
                )
        {
            return !(rc1 == rc2);
        }


        // IOstream Operators

        FoamDynamicMesh_EXPORT friend Ostream& operator<<(Ostream&, const refineCell&);
    };


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_refineCell_Header
