#include <cell.hxx>
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
	Given the cell and a face label, return the opposite face label
	and the face oriented in the same sense as the original face.

\*---------------------------------------------------------------------------*/

#include <oppositeFace.hxx>
#include <boolList.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

tnbLib::label tnbLib::cell::opposingFaceLabel
(
	const label masterFaceLabel,
	const faceUList& meshFaces
) const
{
	// Algorithm:
	// Go through all the faces of the cell and find the one which
	// does not share a single vertex with the master face.  If there
	// are two or more such faces, return the first one and issue a
	// warning; if there is no opposite face, return -1;

	const face& masterFace = meshFaces[masterFaceLabel];

	const labelList& curFaceLabels = *this;

	label oppositeFaceLabel = -1;

	forAll(curFaceLabels, facei)
	{
		// Compare the face with the master
		const face& curFace = meshFaces[curFaceLabels[facei]];

		// Skip the master face
		if
			(
				curFaceLabels[facei] != masterFaceLabel
				&& curFace.size() == masterFace.size()
				)
		{
			bool sharedPoint = false;

			// Compare every vertex of the current face against the
			// vertices of the master face
			forAll(curFace, pointi)
			{
				const label l = curFace[pointi];

				forAll(masterFace, masterPointi)
				{
					if (masterFace[masterPointi] == l)
					{
						sharedPoint = true;
						break;
					}
				}

				if (sharedPoint) break;
			}

			// If no points are shared, this is the opposite face
			if (!sharedPoint)
			{
				if (oppositeFaceLabel == -1)
				{
					// Found opposite face
					oppositeFaceLabel = curFaceLabels[facei];
				}
				else
				{
					// There has already been an opposite face.
					// Non-prismatic cell
					Info << "Multiple faces not sharing vertex: "
						<< oppositeFaceLabel << " and "
						<< curFaceLabels[facei] << endl;
					return -1;
				}
			}
		}
	}

	return oppositeFaceLabel;
}


tnbLib::oppositeFace tnbLib::cell::opposingFace
(
	const label masterFaceLabel,
	const faceUList& meshFaces
) const
{
	// Get the label of the opposite face
	label oppFaceLabel = opposingFaceLabel(masterFaceLabel, meshFaces);

	// If the opposing face is not found, return a failure
	if (oppFaceLabel < 0)
	{
		return oppositeFace(face(0), masterFaceLabel, oppFaceLabel);
	}
	else
	{
		// This is a prismatic cell.  Go through all the vertices of the master
		// face and find an edge going from the master face vertex to a slave
		// face vertex.  If all is OK, there should be only one such
		// edge for every master vertex and will provide te
		// master-to-slave vertex mapping.  Assemble the opposite face
		// in the same manner as the master.

		// Get reference to faces and prepare the return
		const face& masterFace = meshFaces[masterFaceLabel];
		const face& slaveFace = meshFaces[oppFaceLabel];

		// Get cell edges
		const edgeList e = edges(meshFaces);
		boolList usedEdges(e.size(), false);

		oppositeFace oppFace
		(
			face(masterFace.size()),
			masterFaceLabel,
			oppFaceLabel
		);

		forAll(masterFace, pointi)
		{
			// Go through the list of edges and find the edge from this vertex
			// to the slave face
			forAll(e, edgeI)
			{
				if (!usedEdges[edgeI])
				{
					// Get the other vertex
					label otherVertex =
						e[edgeI].otherVertex(masterFace[pointi]);

					if (otherVertex != -1)
					{
						// Found an edge coming from this vertex.
						// Check all vertices of the slave to find out
						// if it exists.
						forAll(slaveFace, slavePointi)
						{
							if (slaveFace[slavePointi] == otherVertex)
							{
								usedEdges[edgeI] = true;
								oppFace[pointi] = otherVertex;

								break;
							}
						}
					}
				}
			}
		}

		return oppFace;
	}
}


// ************************************************************************* //