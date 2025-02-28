/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2018 OpenFOAM Foundation
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

Description
    Create intermediate mesh files from PROSTAR files

\*---------------------------------------------------------------------------*/

#include "starMesh.hxx"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void tnbLib::starMesh::createPolyCells()
{
    // loop through all cell faces and create connectivity. This will produce
    // a global face list and will describe all cells as lists of face labels

    // count the maximum number of faces and set the size of the cellPolys_
    cellPolys_.setSize(cellShapes_.size());

    label maxFaces = 0;

    forAll(cellPolys_, celli)
    {
        cell& curCell = cellPolys_[celli];

        curCell.setSize(cellFaces_[celli].size());

        forAll(curCell, fI)
        {
            curCell[fI] = -1;
        }

        maxFaces += cellFaces_[celli].size();
    }

    Info << "Maximum possible number of faces in mesh: " << maxFaces << endl;

    meshFaces_.setSize(maxFaces);

    // set reference to point-cell addressing
    const labelListList& PointCells = pointCells();

    bool found = false;

    nInternalFaces_ = 0;

    forAll(cellFaces_, celli)
    {
        // Note:
        // Insertion cannot be done in one go as the faces need to be
        // added into the list in the increasing order of neighbour
        // cells.  Therefore, all neighbours will be detected first
        // and then added in the correct order.

        const faceList& curFaces = cellFaces_[celli];

        // Record the neighbour cell
        labelList neiCells(curFaces.size(), -1);

        // Record the face of neighbour cell
        labelList faceOfNeiCell(curFaces.size(), -1);

        label nNeighbours = 0;

        // For all faces ...
        forAll(curFaces, facei)
        {
            // Skip faces that have already been matched
            if (cellPolys_[celli][facei] >= 0) continue;

            found = false;

            const face& curFace = curFaces[facei];

            // get the list of labels
            const labelList& curPoints = curFace;

            // For all points
            forAll(curPoints, pointi)
            {
                // get the list of cells sharing this point
                const labelList& curNeighbours = PointCells[curPoints[pointi]];

                // For all neighbours
                forAll(curNeighbours, neiI)
                {
                    label curNei = curNeighbours[neiI];

                    // reject neighbours with the lower label. This should
                    // also reject current cell.
                    if (curNei > celli)
                    {
                        // get the list of search faces
                        const faceList& searchFaces = cellFaces_[curNei];

                        forAll(searchFaces, neiFacei)
                        {
                            if (searchFaces[neiFacei] == curFace)
                            {
                                // match!!
                                found = true;

                                // Record the neighbour cell and face
                                neiCells[facei] = curNei;
                                faceOfNeiCell[facei] = neiFacei;
                                nNeighbours++;

                                break;
                            }
                        }
                        if (found) break;
                    }
                    if (found) break;
                }
                if (found) break;
            } // End of current points
        } // End of current faces

        // Add the faces in the increasing order of neighbours
        for (label neiSearch = 0; neiSearch < nNeighbours; neiSearch++)
        {
            // Find the lowest neighbour which is still valid
            label nextNei = -1;
            label minNei = cellPolys_.size();

            forAll(neiCells, ncI)
            {
                if (neiCells[ncI] > -1 && neiCells[ncI] < minNei)
                {
                    nextNei = ncI;
                    minNei = neiCells[ncI];
                }
            }

            if (nextNei > -1)
            {
                // Add the face to the list of faces
                meshFaces_[nInternalFaces_] = curFaces[nextNei];

                // Mark for owner
                cellPolys_[celli][nextNei] = nInternalFaces_;

                // Mark for neighbour
                cellPolys_[neiCells[nextNei]][faceOfNeiCell[nextNei]] =
                    nInternalFaces_;

                // Stop the neighbour from being used again
                neiCells[nextNei] = -1;

                // Increment number of faces counter
                nInternalFaces_++;
            }
            else
            {
                FatalErrorInFunction
                    << "Error in internal face insertion"
                    << abort(FatalError);
            }
        }
    }

    // I won't reset the size of internal faces, because more faces will be
    // added in createPolyBoundary()
}


// ************************************************************************* //
