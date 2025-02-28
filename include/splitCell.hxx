#pragma once
#ifndef _splitCell_Header
#define _splitCell_Header

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
    tnbLib::splitCell

Description
    Description of cell after splitting. Contains cellLabel and pointers
    to cells it it split in. See directedRefinement.

SourceFiles
    splitCell.C

\*---------------------------------------------------------------------------*/

#include <label.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

    // Forward declaration of classes

    /*---------------------------------------------------------------------------*\
                               Class splitCell Declaration
    \*---------------------------------------------------------------------------*/

    class splitCell
    {
        // Private Data

            //- Unsplit cell label. Only uptodate if this cell is 'live'
            //  (i.e. no master or slave)
        label celli_;

        //- Parent splitCell or null
        splitCell* parent_;

        //- Cells replacing this or null
        splitCell* master_;

        splitCell* slave_;


    public:

        // Constructors

            //- Construct from cell number and parent
        FoamDynamicMesh_EXPORT splitCell(const label celli, splitCell* parent);

        //- Disallow default bitwise copy construction
        FoamDynamicMesh_EXPORT splitCell(const splitCell&) = delete;


        //- Destructor
        FoamDynamicMesh_EXPORT ~splitCell();


        // Member Functions

            // Access

        label cellLabel() const
        {
            return celli_;
        }

        label& cellLabel()
        {
            return celli_;
        }

        splitCell* parent() const
        {
            return parent_;
        }

        splitCell*& parent()
        {
            return parent_;
        }

        splitCell* master() const
        {
            return master_;
        }

        splitCell*& master()
        {
            return master_;
        }

        splitCell* slave() const
        {
            return slave_;
        }

        splitCell*& slave()
        {
            return slave_;
        }

        //- Check if this is master cell of split
        FoamDynamicMesh_EXPORT bool isMaster() const;

        //- Check if this is unrefined (i.e. has no master or slave)
        FoamDynamicMesh_EXPORT bool isUnrefined() const;

        //- Returns other half of split cell. I.e. slave if this is master.
        FoamDynamicMesh_EXPORT splitCell* getOther() const;


        // Member Operators

            //- Disallow default bitwise assignment
        FoamDynamicMesh_EXPORT void operator=(const splitCell&) = delete;
    };


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_splitCell_Header
