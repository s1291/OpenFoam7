#include "hexBlock.hxx"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

	label hexBlock::vtxLabel(label a, label b, label c) const
	{
		return (a + b * (xDim_ + 1) + c * (xDim_ + 1) * (yDim_ + 1));
	}


	// Calculate the handedness of the block by looking at the orientation
	// of the spanning edges of a hex. Loops until valid cell found (since might
	// be prism)
	void hexBlock::setHandedness()
	{
		const pointField& p = points_;

		for (label k = 0; k <= zDim_ - 1; k++)
		{
			for (label j = 0; j <= yDim_ - 1; j++)
			{
				for (label i = 0; i <= xDim_ - 1; i++)
				{
					vector x = p[vtxLabel(i + 1, j, k)] - p[vtxLabel(i, j, k)];
					vector y = p[vtxLabel(i, j + 1, k)] - p[vtxLabel(i, j, k)];
					vector z = p[vtxLabel(i, j, k + 1)] - p[vtxLabel(i, j, k)];

					if (mag(x) > small && mag(y) > small && mag(z) > small)
					{
						Info << "Looking at cell "
							<< i << ' ' << j << ' ' << k
							<< " to determine orientation."
							<< endl;

						if (((x ^ y) & z) > 0)
						{
							Info << "Right-handed block." << endl;
							blockHandedness_ = right;
						}
						else
						{
							Info << "Left-handed block." << endl;
							blockHandedness_ = left;
						}
						return;
					}
					else
					{
						Info << "Cannot determine orientation of cell "
							<< i << ' ' << j << ' ' << k
							<< " since has base vectors " << x << y << z << endl;
					}
				}
			}
		}

		if (blockHandedness_ == noPoints)
		{
			WarningInFunction
				<< "Cannot determine orientation of block."
				<< " Continuing as if right handed." << endl;
			blockHandedness_ = right;
		}
	}


	// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

	// Construct from components
	hexBlock::hexBlock(const label nx, const label ny, const label nz)
		:
		xDim_(nx - 1),
		yDim_(ny - 1),
		zDim_(nz - 1),
		blockHandedness_(noPoints),
		points_((xDim_ + 1)* (yDim_ + 1)* (zDim_ + 1))
	{}


	// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

	void hexBlock::readPoints
	(
		const bool readBlank,
		const scalar twoDThicknes,
		Istream& is
	)
	{
		scalar iBlank;

		label nPoints = points_.size();

		if (twoDThicknes > 0)
		{
			nPoints /= 2;
		}

		Info << "Reading " << nPoints << " x coordinates..." << endl;
		for (label i = 0; i < nPoints; i++)
		{
			is >> points_[i].x();
		}

		Info << "Reading " << nPoints << " y coordinates..." << endl;
		for (label i = 0; i < nPoints; i++)
		{
			is >> points_[i].y();
		}

		if (twoDThicknes > 0)
		{
			Info << "Extruding " << nPoints << " points in z direction..." << endl;
			// Duplicate points
			for (label i = 0; i < nPoints; i++)
			{
				points_[i + nPoints] = points_[i];
			}
			for (label i = 0; i < nPoints; i++)
			{
				points_[i].z() = 0;
				points_[i + nPoints].z() = twoDThicknes;
			}
		}
		else
		{
			Info << "Reading " << nPoints << " z coordinates..." << endl;
			for (label i = 0; i < nPoints; i++)
			{
				is >> points_[i].z();
			}
		}


		if (readBlank)
		{
			Info << "Reading " << nPoints << " blanks..." << endl;
			for (label i = 0; i < nPoints; i++)
			{
				is >> iBlank;
			}
		}

		// Set left- or righthandedness of block by looking at a cell.
		setHandedness();
	}


	labelListList hexBlock::blockCells() const
	{
		labelListList result(xDim_ * yDim_ * zDim_);

		label cellNo = 0;

		if (blockHandedness_ == right)
		{
			for (label k = 0; k <= zDim_ - 1; k++)
			{
				for (label j = 0; j <= yDim_ - 1; j++)
				{
					for (label i = 0; i <= xDim_ - 1; i++)
					{
						labelList& hexLabels = result[cellNo];
						hexLabels.setSize(8);

						hexLabels[0] = vtxLabel(i, j, k);
						hexLabels[1] = vtxLabel(i + 1, j, k);
						hexLabels[2] = vtxLabel(i + 1, j + 1, k);
						hexLabels[3] = vtxLabel(i, j + 1, k);
						hexLabels[4] = vtxLabel(i, j, k + 1);
						hexLabels[5] = vtxLabel(i + 1, j, k + 1);
						hexLabels[6] = vtxLabel(i + 1, j + 1, k + 1);
						hexLabels[7] = vtxLabel(i, j + 1, k + 1);

						cellNo++;
					}
				}
			}
		}
		else if (blockHandedness_ == left)
		{
			for (label k = 0; k <= zDim_ - 1; k++)
			{
				for (label j = 0; j <= yDim_ - 1; j++)
				{
					for (label i = 0; i <= xDim_ - 1; i++)
					{
						labelList& hexLabels = result[cellNo];
						hexLabels.setSize(8);

						hexLabels[0] = vtxLabel(i, j, k + 1);
						hexLabels[1] = vtxLabel(i + 1, j, k + 1);
						hexLabels[2] = vtxLabel(i + 1, j + 1, k + 1);
						hexLabels[3] = vtxLabel(i, j + 1, k + 1);
						hexLabels[4] = vtxLabel(i, j, k);
						hexLabels[5] = vtxLabel(i + 1, j, k);
						hexLabels[6] = vtxLabel(i + 1, j + 1, k);
						hexLabels[7] = vtxLabel(i, j + 1, k);

						cellNo++;
					}
				}
			}
		}
		else
		{
			FatalErrorInFunction
				<< "Unable to determine block handedness as points "
				<< "have not been read in yet"
				<< abort(FatalError);
		}

		return result;
	}


	// Return block patch faces given direction and range limits
	// From the cfx manual: direction
	// 0 = solid (3-D patch),
	// 1 = high i, 2 = high j, 3 = high k
	// 4 = low i, 5 = low j, 6 = low k
	faceList hexBlock::patchFaces(const label direc, const labelList& range) const
	{
		if (range.size() != 6)
		{
			FatalErrorInFunction
				<< "Invalid size of the range array: " << range.size()
				<< ". Should be 6 (xMin, xMax, yMin, yMax, zMin, zMax"
				<< abort(FatalError);
		}

		label xMinRange = range[0];
		label xMaxRange = range[1];
		label yMinRange = range[2];
		label yMaxRange = range[3];
		label zMinRange = range[4];
		label zMaxRange = range[5];

		faceList result(0);

		switch (direc)
		{
		case 1:
		{
			// high i = xmax

			result.setSize
			(
				(yMaxRange - yMinRange + 1) * (zMaxRange - zMinRange + 1)
			);

			label p = 0;
			for (label k = zMinRange - 1; k <= zMaxRange - 1; k++)
			{
				for (label j = yMinRange - 1; j <= yMaxRange - 1; j++)
				{
					result[p].setSize(4);

					// set the points
					result[p][0] = vtxLabel(xDim_, j, k);
					result[p][1] = vtxLabel(xDim_, j + 1, k);
					result[p][2] = vtxLabel(xDim_, j + 1, k + 1);
					result[p][3] = vtxLabel(xDim_, j, k + 1);

					p++;
				}
			}

			result.setSize(p);
			break;
		}

		case 2:
		{
			// high j = ymax
			result.setSize
			(
				(xMaxRange - xMinRange + 1) * (zMaxRange - zMinRange + 1)
			);

			label p = 0;
			for (label i = xMinRange - 1; i <= xMaxRange - 1; i++)
			{
				for (label k = zMinRange - 1; k <= zMaxRange - 1; k++)
				{
					result[p].setSize(4);

					// set the points
					result[p][0] = vtxLabel(i, yDim_, k);
					result[p][1] = vtxLabel(i, yDim_, k + 1);
					result[p][2] = vtxLabel(i + 1, yDim_, k + 1);
					result[p][3] = vtxLabel(i + 1, yDim_, k);

					p++;
				}
			}

			result.setSize(p);
			break;
		}

		case 3:
		{
			// high k = zmax
			result.setSize
			(
				(xMaxRange - xMinRange + 1) * (yMaxRange - yMinRange + 1)
			);

			label p = 0;
			for (label i = xMinRange - 1; i <= xMaxRange - 1; i++)
			{
				for (label j = yMinRange - 1; j <= yMaxRange - 1; j++)
				{
					result[p].setSize(4);

					// set the points
					result[p][0] = vtxLabel(i, j, zDim_);
					result[p][1] = vtxLabel(i + 1, j, zDim_);
					result[p][2] = vtxLabel(i + 1, j + 1, zDim_);
					result[p][3] = vtxLabel(i, j + 1, zDim_);

					p++;
				}
			}

			result.setSize(p);
			break;
		}

		case 4:
		{
			// low i = xmin
			result.setSize
			(
				(yMaxRange - yMinRange + 1) * (zMaxRange - zMinRange + 1)
			);

			label p = 0;
			for (label k = zMinRange - 1; k <= zMaxRange - 1; k++)
			{
				for (label j = yMinRange - 1; j <= yMaxRange - 1; j++)
				{
					result[p].setSize(4);

					// set the points
					result[p][0] = vtxLabel(0, j, k);
					result[p][1] = vtxLabel(0, j, k + 1);
					result[p][2] = vtxLabel(0, j + 1, k + 1);
					result[p][3] = vtxLabel(0, j + 1, k);

					p++;
				}
			}

			result.setSize(p);
			break;
		}

		case 5:
		{
			// low j = ymin
			result.setSize
			(
				(xMaxRange - xMinRange + 1) * (zMaxRange - zMinRange + 1)
			);

			label p = 0;
			for (label i = xMinRange - 1; i <= xMaxRange - 1; i++)
			{
				for (label k = zMinRange - 1; k <= zMaxRange - 1; k++)
				{
					result[p].setSize(4);

					// set the points
					result[p][0] = vtxLabel(i, 0, k);
					result[p][1] = vtxLabel(i + 1, 0, k);
					result[p][2] = vtxLabel(i + 1, 0, k + 1);
					result[p][3] = vtxLabel(i, 0, k + 1);

					p++;
				}
			}

			result.setSize(p);
			break;
		}

		case 6:
		{
			// low k = zmin
			result.setSize
			(
				(xMaxRange - xMinRange + 1) * (yMaxRange - yMinRange + 1)
			);

			label p = 0;
			for (label i = xMinRange - 1; i <= xMaxRange - 1; i++)
			{
				for (label j = yMinRange - 1; j <= yMaxRange - 1; j++)
				{
					result[p].setSize(4);

					// set the points
					result[p][0] = vtxLabel(i, j, 0);
					result[p][1] = vtxLabel(i, j + 1, 0);
					result[p][2] = vtxLabel(i + 1, j + 1, 0);
					result[p][3] = vtxLabel(i + 1, j, 0);

					p++;
				}
			}

			result.setSize(p);
			break;
		}

		default:
		{
			FatalErrorInFunction
				<< "direction out of range (1 to 6): " << direc
				<< abort(FatalError);
		}
		}

		// Correct the face orientation based on the handedness of the block.
		// Do nothing for the right-handed block
		if (blockHandedness_ == noPoints)
		{
			FatalErrorInFunction
				<< "Unable to determine block handedness as points "
				<< "have not been read in yet"
				<< abort(FatalError);
		}
		else if (blockHandedness_ == left)
		{
			// turn all faces inside out
			forAll(result, facei)
			{
				result[facei].flip();
			}
		}

		return result;
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //