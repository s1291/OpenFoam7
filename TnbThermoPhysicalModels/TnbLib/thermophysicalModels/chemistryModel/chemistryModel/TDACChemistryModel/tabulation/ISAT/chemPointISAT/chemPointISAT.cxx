#include <chemPointISAT.hxx>

#include <SVD.hxx>
#include <dictionary.hxx>  // added by amir

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

// Defined as static to be able to dynamically change it during simulations
// (all chemPoints refer to the same object)
template<class CompType, class ThermoType>
tnbLib::scalar tnbLib::chemPointISAT<CompType, ThermoType>::tolerance_;

// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

template<class CompType, class ThermoType>
void tnbLib::chemPointISAT<CompType, ThermoType>::qrDecompose
(
	const label nCols,
	scalarSquareMatrix& R
)
{
	scalarField c(nCols);
	scalarField d(nCols);
	scalar scale, sigma, sum;

	for (label k = 0; k < nCols - 1; k++)
	{
		scale = 0;
		for (label i = k; i < nCols; i++)
		{
			scale = max(scale, mag(R(i, k)));
		}
		if (scale == 0)
		{
			c[k] = d[k] = 0;
		}
		else
		{
			for (label i = k; i < nCols; i++)
			{
				R(i, k) /= scale;
			}
			sum = 0;
			for (label i = k; i < nCols; i++)
			{
				sum += sqr(R(i, k));
			}
			sigma = sign(R(k, k))*sqrt(sum);
			R(k, k) += sigma;
			c[k] = sigma * R(k, k);
			d[k] = -scale * sigma;
			for (label j = k + 1; j < nCols; j++)
			{
				sum = 0;
				for (label i = k; i < nCols; i++)
				{
					sum += R(i, k)*R(i, j);
				}
				scalar tau = sum / c[k];
				for (label i = k; i < nCols; i++)
				{
					R(i, j) -= tau * R(i, k);
				}
			}
		}
	}

	d[nCols - 1] = R(nCols - 1, nCols - 1);

	// form R
	for (label i = 0; i < nCols; i++)
	{
		R(i, i) = d[i];
		for (label j = 0; j < i; j++)
		{
			R(i, j) = 0;
		}
	}
}


template<class CompType, class ThermoType>
void tnbLib::chemPointISAT<CompType, ThermoType>::qrUpdate
(
	scalarSquareMatrix& R,
	const label n,
	const tnbLib::scalarField &u,
	const tnbLib::scalarField &v
)
{
	label k;

	scalarField w(u);
	for (k = n - 1; k >= 0; k--)
	{
		if (w[k] != 0)
		{
			break;
		}
	}

	if (k < 0)
	{
		k = 0;
	}

	for (label i = k - 1; i >= 0; i--)
	{
		rotate(R, i, w[i], -w[i + 1], n);
		if (w[i] == 0)
		{
			w[i] = mag(w[i + 1]);
		}
		else if (mag(w[i]) > mag(w[i + 1]))
		{
			w[i] = mag(w[i])*sqrt(1.0 + sqr(w[i + 1] / w[i]));
		}
		else
		{
			w[i] = mag(w[i + 1])*sqrt(1.0 + sqr(w[i] / w[i + 1]));
		}
	}

	for (label i = 0; i < n; i++)
	{
		R(0, i) += w[0] * v[i];
	}

	for (label i = 0; i < k; i++)
	{
		rotate(R, i, R(i, i), -R(i + 1, i), n);
	}
}


template<class CompType, class ThermoType>
void tnbLib::chemPointISAT<CompType, ThermoType>::rotate
(
	scalarSquareMatrix& R,
	const label i,
	const scalar a,
	const scalar b,
	label n
)
{
	scalar c, fact, s, w, y;
	if (a == 0)
	{
		c = 0;
		s = (b >= 0 ? 1 : -1);
	}
	else if (mag(a) > mag(b))
	{
		fact = b / a;
		c = sign(a) / sqrt(1.0 + sqr(fact));
		s = fact * c;
	}
	else
	{
		fact = a / b;
		s = sign(b) / sqrt(1.0 + sqr(fact));
		c = fact * s;
	}
	for (label j = i; j < n; j++)
	{
		y = R(i, j);
		w = R(i + 1, j);
		R(i, j) = c * y - s * w;
		R(i + 1, j) = s * y + c * w;
	}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CompType, class ThermoType>
tnbLib::chemPointISAT<CompType, ThermoType>::chemPointISAT
(
	TDACChemistryModel<CompType, ThermoType>& chemistry,
	const scalarField& phi,
	const scalarField& Rphi,
	const scalarSquareMatrix& A,
	const scalarField& scaleFactor,
	const scalar& tolerance,
	const label& completeSpaceSize,
	const dictionary& coeffsDict,
	binaryNode<CompType, ThermoType>* node
)
	:
	chemistry_(chemistry),
	phi_(phi),
	Rphi_(Rphi),
	A_(A),
	scaleFactor_(scaleFactor),
	node_(node),
	completeSpaceSize_(completeSpaceSize),
	nGrowth_(0),
	nActiveSpecies_(chemistry.mechRed()->NsSimp()),
	simplifiedToCompleteIndex_(nActiveSpecies_),
	timeTag_(chemistry_.timeSteps()),
	lastTimeUsed_(chemistry_.timeSteps()),
	toRemove_(false),
	maxNumNewDim_(coeffsDict.lookupOrDefault("maxNumNewDim", 0)),
	printProportion_(coeffsDict.lookupOrDefault("printProportion", false)),
	numRetrieve_(0),
	nLifeTime_(0),
	completeToSimplifiedIndex_
	(
		completeSpaceSize - (2 + (variableTimeStep() == 1 ? 1 : 0))
	)
{
	tolerance_ = tolerance;

	if (variableTimeStep())
	{
		nAdditionalEqns_ = 3;
		iddeltaT_ = completeSpaceSize - 1;
		scaleFactor_[iddeltaT_] *= phi_[iddeltaT_] / tolerance_;
	}
	else
	{
		nAdditionalEqns_ = 2;
		iddeltaT_ = completeSpaceSize; // will not be used
	}
	idT_ = completeSpaceSize - nAdditionalEqns_;
	idp_ = completeSpaceSize - nAdditionalEqns_ + 1;

	bool isMechRedActive = chemistry_.mechRed()->active();
	if (isMechRedActive)
	{
		for (label i = 0; i < completeSpaceSize - nAdditionalEqns_; i++)
		{
			completeToSimplifiedIndex_[i] =
				chemistry.completeToSimplifiedIndex()[i];
		}
		for (label i = 0; i < nActiveSpecies_; i++)
		{
			simplifiedToCompleteIndex_[i] =
				chemistry.simplifiedToCompleteIndex()[i];
		}
	}

	label reduOrCompDim = completeSpaceSize;
	if (isMechRedActive)
	{
		reduOrCompDim = nActiveSpecies_ + nAdditionalEqns_;
	}

	// SVD decomposition A = U*D*V^T
	SVD svdA(A);

	scalarDiagonalMatrix D(reduOrCompDim);
	const scalarDiagonalMatrix& S = svdA.S();

	// Replace the value of vector D by max(D, 1/2), first ISAT paper
	for (label i = 0; i < reduOrCompDim; i++)
	{
		D[i] = max(S[i], 0.5);
	}

	// Rebuild A with max length, tol and scale factor before QR decomposition
	scalarRectangularMatrix Atilde(reduOrCompDim);

	// Result stored in Atilde
	multiply(Atilde, svdA.U(), D, svdA.V().T());

	for (label i = 0; i < reduOrCompDim; i++)
	{
		for (label j = 0; j < reduOrCompDim; j++)
		{
			label compi = i;

			if (isMechRedActive)
			{
				compi = simplifiedToCompleteIndex(i);
			}

			// SF*A/tolerance
			// (where SF is diagonal with inverse of scale factors)
			// SF*A is the same as dividing each line by the scale factor
			// corresponding to the species of this line
			Atilde(i, j) /= (tolerance*scaleFactor[compi]);
		}
	}

	// The object LT_ (the transpose of the Q) describe the EOA, since we have
	// A^T B^T B A that should be factorized into L Q^T Q L^T and is set in the
	// qrDecompose function
	LT_ = scalarSquareMatrix(Atilde);

	qrDecompose(reduOrCompDim, LT_);
}


template<class CompType, class ThermoType>
tnbLib::chemPointISAT<CompType, ThermoType>::chemPointISAT
(
	tnbLib::chemPointISAT<CompType, ThermoType>& p
)
	:
	chemistry_(p.chemistry()),
	phi_(p.phi()),
	Rphi_(p.Rphi()),
	LT_(p.LT()),
	A_(p.A()),
	scaleFactor_(p.scaleFactor()),
	node_(p.node()),
	completeSpaceSize_(p.completeSpaceSize()),
	nGrowth_(p.nGrowth()),
	nActiveSpecies_(p.nActiveSpecies()),
	simplifiedToCompleteIndex_(p.simplifiedToCompleteIndex()),
	timeTag_(p.timeTag()),
	lastTimeUsed_(p.lastTimeUsed()),
	toRemove_(p.toRemove()),
	maxNumNewDim_(p.maxNumNewDim()),
	numRetrieve_(0),
	nLifeTime_(0),
	completeToSimplifiedIndex_(p.completeToSimplifiedIndex())
{
	tolerance_ = p.tolerance();

	if (variableTimeStep())
	{
		nAdditionalEqns_ = 3;
		idT_ = completeSpaceSize() - 3;
		idp_ = completeSpaceSize() - 2;
		iddeltaT_ = completeSpaceSize() - 1;
	}
	else
	{
		nAdditionalEqns_ = 2;
		idT_ = completeSpaceSize() - 2;
		idp_ = completeSpaceSize() - 1;
		iddeltaT_ = completeSpaceSize(); // will not be used
	}
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CompType, class ThermoType>
bool tnbLib::chemPointISAT<CompType, ThermoType>::inEOA(const scalarField& phiq)
{
	scalarField dphi(phiq - phi());
	bool isMechRedActive = chemistry_.mechRed()->active();
	label dim(0);
	if (isMechRedActive)
	{
		dim = nActiveSpecies_;
	}
	else
	{
		dim = completeSpaceSize() - nAdditionalEqns_;
	}

	scalar epsTemp = 0;
	List<scalar> propEps(completeSpaceSize(), scalar(0));

	for (label i = 0; i < completeSpaceSize() - nAdditionalEqns_; i++)
	{
		scalar temp = 0;

		// When mechanism reduction is inactive OR on active species multiply L
		// by dphi to get the distance in the active species direction else (for
		// inactive species), just multiply the diagonal element and dphi
		if
			(
				!(isMechRedActive)
				|| (isMechRedActive && completeToSimplifiedIndex_[i] != -1)
				)
		{
			label si = (isMechRedActive) ? completeToSimplifiedIndex_[i] : i;

			for (label j = si; j < dim; j++)// LT is upper triangular
			{
				label sj = (isMechRedActive) ? simplifiedToCompleteIndex_[j] : j;
				temp += LT_(si, j)*dphi[sj];
			}

			temp += LT_(si, dim)*dphi[idT_];
			temp += LT_(si, dim + 1)*dphi[idp_];
			if (variableTimeStep())
			{
				temp += LT_(si, dim + 2)*dphi[iddeltaT_];
			}
		}
		else
		{
			temp = dphi[i] / (tolerance_*scaleFactor_[i]);
		}

		epsTemp += sqr(temp);

		if (printProportion_)
		{
			propEps[i] = temp;
		}
	}

	// Temperature
	if (variableTimeStep())
	{
		epsTemp +=
			sqr
			(
				LT_(dim, dim)*dphi[idT_]
				+ LT_(dim, dim + 1)*dphi[idp_]
				+ LT_(dim, dim + 2)*dphi[iddeltaT_]
			);
	}
	else
	{
		epsTemp +=
			sqr
			(
				LT_(dim, dim)*dphi[idT_]
				+ LT_(dim, dim + 1)*dphi[idp_]
			);
	}

	// Pressure
	if (variableTimeStep())
	{
		epsTemp +=
			sqr
			(
				LT_(dim + 1, dim + 1)*dphi[idp_]
				+ LT_(dim + 1, dim + 2)*dphi[iddeltaT_]
			);
	}
	else
	{
		epsTemp += sqr(LT_(dim + 1, dim + 1)*dphi[idp_]);
	}

	if (variableTimeStep())
	{
		epsTemp += sqr(LT_[dim + 2][dim + 2] * dphi[iddeltaT_]);
	}

	if (printProportion_)
	{
		propEps[idT_] = sqr
		(
			LT_(dim, dim)*dphi[idT_]
			+ LT_(dim, dim + 1)*dphi[idp_]
		);

		propEps[idp_] =
			sqr(LT_(dim + 1, dim + 1)*dphi[idp_]);

		if (variableTimeStep())
		{
			propEps[iddeltaT_] =
				sqr(LT_[dim + 2][dim + 2] * dphi[iddeltaT_]);
		}

	}

	if (sqrt(epsTemp) > 1 + tolerance_)
	{
		if (printProportion_)
		{
			scalar max = -1;
			label maxIndex = -1;
			for (label i = 0; i < completeSpaceSize(); i++)
			{
				if (max < propEps[i])
				{
					max = propEps[i];
					maxIndex = i;
				}
			}
			word propName;
			if (maxIndex >= completeSpaceSize() - nAdditionalEqns_)
			{
				if (maxIndex == idT_)
				{
					propName = "T";
				}
				else if (maxIndex == idp_)
				{
					propName = "p";
				}
				else if (maxIndex == iddeltaT_)
				{
					propName = "deltaT";
				}
			}
			else
			{
				propName = chemistry_.Y()[maxIndex].member();
			}
			Info << "Direction maximum impact to error in ellipsoid: "
				<< propName << endl;
			Info << "Proportion to the total error on the retrieve: "
				<< max / (epsTemp + small) << endl;
		}
		return false;
	}
	else
	{
		return true;
	}
}


template<class CompType, class ThermoType>
bool tnbLib::chemPointISAT<CompType, ThermoType>::checkSolution
(
	const scalarField& phiq,
	const scalarField& Rphiq
)
{
	scalar eps2 = 0;
	scalarField dR(Rphiq - Rphi());
	scalarField dphi(phiq - phi());
	const scalarField& scaleFactorV(scaleFactor());
	const scalarSquareMatrix& Avar(A());
	bool isMechRedActive = chemistry_.mechRed()->active();
	scalar dRl = 0;
	label dim = completeSpaceSize() - 2;
	if (isMechRedActive)
	{
		dim = nActiveSpecies_;
	}

	// Since we build only the solution for the species, T and p are not
	// included
	for (label i = 0; i < completeSpaceSize() - nAdditionalEqns_; i++)
	{
		dRl = 0;
		if (isMechRedActive)
		{
			label si = completeToSimplifiedIndex_[i];

			// If this species is active
			if (si != -1)
			{
				for (label j = 0; j < dim; j++)
				{
					label sj = simplifiedToCompleteIndex_[j];
					dRl += Avar(si, j)*dphi[sj];
				}
				dRl += Avar(si, nActiveSpecies_)*dphi[idT_];
				dRl += Avar(si, nActiveSpecies_ + 1)*dphi[idp_];
				if (variableTimeStep())
				{
					dRl += Avar(si, nActiveSpecies_ + 2)*dphi[iddeltaT_];
				}
			}
			else
			{
				dRl = dphi[i];
			}
		}
		else
		{
			for (label j = 0; j < completeSpaceSize(); j++)
			{
				dRl += Avar(i, j)*dphi[j];
			}
		}
		eps2 += sqr((dR[i] - dRl) / scaleFactorV[i]);
	}

	eps2 = sqrt(eps2);
	if (eps2 > tolerance())
	{
		return false;
	}
	else
	{
		// if the solution is in the ellipsoid of accuracy
		return true;
	}
}


template<class CompType, class ThermoType>
bool tnbLib::chemPointISAT<CompType, ThermoType>::grow(const scalarField& phiq)
{
	scalarField dphi(phiq - phi());
	label dim = completeSpaceSize();
	label initNActiveSpecies(nActiveSpecies_);
	bool isMechRedActive = chemistry_.mechRed()->active();

	if (isMechRedActive)
	{
		label activeAdded(0);
		DynamicList<label> dimToAdd(0);

		// check if the difference of active species is lower than the maximum
		// number of new dimensions allowed
		for (label i = 0; i < completeSpaceSize() - nAdditionalEqns_; i++)
		{
			// first test if the current chemPoint has an inactive species
			// corresponding to an active one in the query point
			if
				(
					completeToSimplifiedIndex_[i] == -1
					&& chemistry_.completeToSimplifiedIndex()[i] != -1
					)
			{
				activeAdded++;
				dimToAdd.append(i);
			}
			// then test if an active species in the current chemPoint
			// corresponds to an inactive on the query side
			if
				(
					completeToSimplifiedIndex_[i] != -1
					&& chemistry_.completeToSimplifiedIndex()[i] == -1
					)
			{
				activeAdded++;
				// we don't need to add a new dimension but we count it to have
				// control on the difference through maxNumNewDim
			}
			// finally test if both points have inactive species but
			// with a dphi!=0
			if
				(
					completeToSimplifiedIndex_[i] == -1
					&& chemistry_.completeToSimplifiedIndex()[i] == -1
					&& dphi[i] != 0
					)
			{
				activeAdded++;
				dimToAdd.append(i);
			}
		}

		// if the number of added dimension is too large, growth fail
		if (activeAdded > maxNumNewDim_)
		{
			return false;
		}

		// the number of added dimension to the current chemPoint
		nActiveSpecies_ += dimToAdd.size();
		simplifiedToCompleteIndex_.setSize(nActiveSpecies_);
		forAll(dimToAdd, i)
		{
			label si = nActiveSpecies_ - dimToAdd.size() + i;
			// add the new active species
			simplifiedToCompleteIndex_[si] = dimToAdd[i];
			completeToSimplifiedIndex_[dimToAdd[i]] = si;
		}

		// update LT and A :
		//-add new column and line for the new active species
		//-transfer last two lines of the previous matrix (p and T) to the end
		//  (change the diagonal position)
		//-set all element of the new lines and columns to zero except diagonal
		//  (=1/(tolerance*scaleFactor))
		if (nActiveSpecies_ > initNActiveSpecies)
		{
			scalarSquareMatrix LTvar = LT_; // take a copy of LT_
			scalarSquareMatrix Avar = A_; // take a copy of A_
			LT_ = scalarSquareMatrix(nActiveSpecies_ + nAdditionalEqns_, Zero);
			A_ = scalarSquareMatrix(nActiveSpecies_ + nAdditionalEqns_, Zero);

			// write the initial active species
			for (label i = 0; i < initNActiveSpecies; i++)
			{
				for (label j = 0; j < initNActiveSpecies; j++)
				{
					LT_(i, j) = LTvar(i, j);
					A_(i, j) = Avar(i, j);
				}
			}

			// write the columns for temperature and pressure
			for (label i = 0; i < initNActiveSpecies; i++)
			{
				for (label j = 1; j >= 0; j--)
				{
					LT_(i, nActiveSpecies_ + j) = LTvar(i, initNActiveSpecies + j);
					A_(i, nActiveSpecies_ + j) = Avar(i, initNActiveSpecies + j);
					LT_(nActiveSpecies_ + j, i) = LTvar(initNActiveSpecies + j, i);
					A_(nActiveSpecies_ + j, i) = Avar(initNActiveSpecies + j, i);
				}
			}
			// end with the diagonal elements for temperature and pressure
			LT_(nActiveSpecies_, nActiveSpecies_) =
				LTvar(initNActiveSpecies, initNActiveSpecies);
			A_(nActiveSpecies_, nActiveSpecies_) =
				Avar(initNActiveSpecies, initNActiveSpecies);
			LT_(nActiveSpecies_ + 1, nActiveSpecies_ + 1) =
				LTvar(initNActiveSpecies + 1, initNActiveSpecies + 1);
			A_(nActiveSpecies_ + 1, nActiveSpecies_ + 1) =
				Avar(initNActiveSpecies + 1, initNActiveSpecies + 1);

			if (variableTimeStep())
			{
				LT_(nActiveSpecies_ + 2, nActiveSpecies_ + 2) =
					LTvar(initNActiveSpecies + 2, initNActiveSpecies + 2);
				A_(nActiveSpecies_ + 2, nActiveSpecies_ + 2) =
					Avar(initNActiveSpecies + 2, initNActiveSpecies + 2);
			}

			for (label i = initNActiveSpecies; i < nActiveSpecies_; i++)
			{
				LT_(i, i) =
					1.0
					/ (tolerance_*scaleFactor_[simplifiedToCompleteIndex_[i]]);
				A_(i, i) = 1;
			}
		}

		dim = nActiveSpecies_ + nAdditionalEqns_;
	}

	// beginning of grow algorithm
	scalarField phiTilde(dim, 0);
	scalar normPhiTilde = 0;
	// p' = L^T.(p-phi)

	for (label i = 0; i < dim; i++)
	{
		for (label j = i; j < dim - nAdditionalEqns_; j++)// LT is upper triangular
		{
			label sj = j;
			if (isMechRedActive)
			{
				sj = simplifiedToCompleteIndex_[j];
			}
			phiTilde[i] += LT_(i, j)*dphi[sj];
		}

		phiTilde[i] += LT_(i, dim - nAdditionalEqns_)*dphi[idT_];
		phiTilde[i] += LT_(i, dim - nAdditionalEqns_ + 1)*dphi[idp_];

		if (variableTimeStep())
		{
			phiTilde[i] += LT_(i, dim - nAdditionalEqns_ + 2)*dphi[iddeltaT_];
		}
		normPhiTilde += sqr(phiTilde[i]);
	}

	scalar invSqrNormPhiTilde = 1.0 / normPhiTilde;
	normPhiTilde = sqrt(normPhiTilde);

	// gamma = (1/|p'| - 1)/|p'|^2
	scalar gamma = (1 / normPhiTilde - 1)*invSqrNormPhiTilde;
	scalarField u(gamma*phiTilde);
	scalarField v(dim, 0);

	for (label i = 0; i < dim; i++)
	{
		for (label j = 0; j <= i; j++)
		{
			v[i] += phiTilde[j] * LT_(j, i);
		}
	}

	qrUpdate(LT_, dim, u, v);
	nGrowth_++;

	return true;
}


template<class CompType, class ThermoType>
void tnbLib::chemPointISAT<CompType, ThermoType>::increaseNumRetrieve()
{
	this->numRetrieve_++;
}


template<class CompType, class ThermoType>
void tnbLib::chemPointISAT<CompType, ThermoType>::resetNumRetrieve()
{
	this->numRetrieve_ = 0;
}


template<class CompType, class ThermoType>
void tnbLib::chemPointISAT<CompType, ThermoType>::increaseNLifeTime()
{
	this->nLifeTime_++;
}


template<class CompType, class ThermoType>
tnbLib::label tnbLib::chemPointISAT<CompType, ThermoType>::
simplifiedToCompleteIndex
(
	const label i
)
{
	if (i < nActiveSpecies_)
	{
		return simplifiedToCompleteIndex_[i];
	}
	else if (i == nActiveSpecies_)
	{
		return completeSpaceSize_ - nAdditionalEqns_;
	}
	else if (i == nActiveSpecies_ + 1)
	{
		return completeSpaceSize_ - nAdditionalEqns_ + 1;
	}
	else if (variableTimeStep() && (i == nActiveSpecies_ + 2))
	{
		return completeSpaceSize_ - nAdditionalEqns_ + 2;
	}
	else
	{
		return -1;
	}
}


// ************************************************************************* //