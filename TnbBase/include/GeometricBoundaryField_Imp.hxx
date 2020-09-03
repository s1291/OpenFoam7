#pragma once
#include <emptyPolyPatch.hxx>
#include <commSchedule.hxx>
#include <globalMeshData.hxx>
#include <cyclicPolyPatch.hxx>

template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
readField
(
	const DimensionedField<Type, GeoMesh>& field,
	const dictionary& dict
)
{
	// Clear the boundary field if already initialised
	this->clear();

	this->setSize(bmesh_.size());

	if (debug)
	{
		InfoInFunction << endl;
	}


	label nUnset = this->size();

	// 1. Handle explicit patch names. Note that there can be only one explicit
	//    patch name since is key of dictionary.
	forAllConstIter(dictionary, dict, iter)
	{
		if (iter().isDict() && !iter().keyword().isPattern())
		{
			label patchi = bmesh_.findPatchID(iter().keyword());

			if (patchi != -1)
			{
				this->set
				(
					patchi,
					PatchField<Type>::New
					(
						bmesh_[patchi],
						field,
						iter().dict()
					)
				);
				nUnset--;
			}
		}
	}

	if (nUnset == 0)
	{
		return;
	}


	// 2. Patch-groups. (using non-wild card entries of dictionaries)
	// (patchnames already matched above)
	// Note: in reverse order of entries in the dictionary (last
	// patchGroups wins). This is so is consistent with dictionary wildcard
	// behaviour
	if (dict.size())
	{
		for
			(
				IDLList<entry>::const_reverse_iterator iter = dict.rbegin();
				iter != dict.rend();
				++iter
				)
		{
			const entry& e = iter();

			if (e.isDict() && !e.keyword().isPattern())
			{
				const labelList patchIDs = bmesh_.findIndices
				(
					e.keyword(),
					true                    // use patchGroups
				);

				forAll(patchIDs, i)
				{
					label patchi = patchIDs[i];

					if (!this->set(patchi))
					{
						this->set
						(
							patchi,
							PatchField<Type>::New
							(
								bmesh_[patchi],
								field,
								e.dict()
							)
						);
					}
				}
			}
		}
	}


	// 3. Wildcard patch overrides
	forAll(bmesh_, patchi)
	{
		if (!this->set(patchi))
		{
			if (bmesh_[patchi].type() == emptyPolyPatch::typeName)
			{
				this->set
				(
					patchi,
					PatchField<Type>::New
					(
						emptyPolyPatch::typeName,
						bmesh_[patchi],
						field
					)
				);
			}
			else
			{
				bool found = dict.found(bmesh_[patchi].name());

				if (found)
				{
					this->set
					(
						patchi,
						PatchField<Type>::New
						(
							bmesh_[patchi],
							field,
							dict.subDict(bmesh_[patchi].name())
						)
					);
				}
			}
		}
	}


	// Check for any unset patches
	forAll(bmesh_, patchi)
	{
		if (!this->set(patchi))
		{
			if (bmesh_[patchi].type() == cyclicPolyPatch::typeName)
			{
				FatalIOErrorInFunction
				(
					dict
				) << "Cannot find patchField entry for cyclic "
					<< bmesh_[patchi].name() << endl
					<< "Is your field uptodate with split cyclics?" << endl
					<< "Run foamUpgradeCyclics to convert mesh and fields"
					<< " to split cyclics." << exit(FatalIOError);
			}
			else
			{
				FatalIOErrorInFunction
				(
					dict
				) << "Cannot find patchField entry for "
					<< bmesh_[patchi].name() << exit(FatalIOError);
			}
		}
	}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
Boundary
(
	const BoundaryMesh& bmesh
)
	:
	FieldField<PatchField, Type>(bmesh.size()),
	bmesh_(bmesh)
{}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
Boundary
(
	const BoundaryMesh& bmesh,
	const DimensionedField<Type, GeoMesh>& field,
	const word& patchFieldType
)
	:
	FieldField<PatchField, Type>(bmesh.size()),
	bmesh_(bmesh)
{
	if (debug)
	{
		InfoInFunction << endl;
	}

	forAll(bmesh_, patchi)
	{
		this->set
		(
			patchi,
			PatchField<Type>::New
			(
				patchFieldType,
				bmesh_[patchi],
				field
			)
		);
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
Boundary
(
	const BoundaryMesh& bmesh,
	const DimensionedField<Type, GeoMesh>& field,
	const wordList& patchFieldTypes,
	const wordList& constraintTypes
)
	:
	FieldField<PatchField, Type>(bmesh.size()),
	bmesh_(bmesh)
{
	if (debug)
	{
		InfoInFunction << endl;
	}

	if
		(
			patchFieldTypes.size() != this->size()
			|| (constraintTypes.size() && (constraintTypes.size() != this->size()))
			)
	{
		FatalErrorInFunction
			<< "Incorrect number of patch type specifications given" << nl
			<< "    Number of patches in mesh = " << bmesh.size()
			<< " number of patch type specifications = "
			<< patchFieldTypes.size()
			<< abort(FatalError);
	}

	if (constraintTypes.size())
	{
		forAll(bmesh_, patchi)
		{
			this->set
			(
				patchi,
				PatchField<Type>::New
				(
					patchFieldTypes[patchi],
					constraintTypes[patchi],
					bmesh_[patchi],
					field
				)
			);
		}
	}
	else
	{
		forAll(bmesh_, patchi)
		{
			this->set
			(
				patchi,
				PatchField<Type>::New
				(
					patchFieldTypes[patchi],
					bmesh_[patchi],
					field
				)
			);
		}
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
Boundary
(
	const BoundaryMesh& bmesh,
	const DimensionedField<Type, GeoMesh>& field,
	const PtrList<PatchField<Type>>& ptfl
)
	:
	FieldField<PatchField, Type>(bmesh.size()),
	bmesh_(bmesh)
{
	if (debug)
	{
		InfoInFunction << endl;
	}

	forAll(bmesh_, patchi)
	{
		this->set(patchi, ptfl[patchi].clone(field));
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
Boundary
(
	const DimensionedField<Type, GeoMesh>& field,
	const typename GeometricField<Type, PatchField, GeoMesh>::
	Boundary& btf
)
	:
	FieldField<PatchField, Type>(btf.size()),
	bmesh_(btf.bmesh_)
{
	if (debug)
	{
		InfoInFunction << endl;
	}

	forAll(bmesh_, patchi)
	{
		this->set(patchi, btf[patchi].clone(field));
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
Boundary
(
	const typename GeometricField<Type, PatchField, GeoMesh>::
	Boundary& btf
)
	:
	FieldField<PatchField, Type>(btf),
	bmesh_(btf.bmesh_)
{
	if (debug)
	{
		InfoInFunction << endl;
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
Boundary
(
	typename GeometricField<Type, PatchField, GeoMesh>::
	Boundary&& btf
)
	:
	FieldField<PatchField, Type>(move(btf)),
	bmesh_(btf.bmesh_)
{
	if (debug)
	{
		InfoInFunction << endl;
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
Boundary
(
	const BoundaryMesh& bmesh,
	const DimensionedField<Type, GeoMesh>& field,
	const dictionary& dict
)
	:
	FieldField<PatchField, Type>(bmesh.size()),
	bmesh_(bmesh)
{
	readField(field, dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
updateCoeffs()
{
	if (debug)
	{
		InfoInFunction << endl;
	}

	forAll(*this, patchi)
	{
		this->operator[](patchi).updateCoeffs();
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
evaluate()
{
	if (debug)
	{
		InfoInFunction << endl;
	}

	if
		(
			Pstream::defaultCommsType == Pstream::commsTypes::blocking
			|| Pstream::defaultCommsType == Pstream::commsTypes::nonBlocking
			)
	{
		label nReq = Pstream::nRequests();

		forAll(*this, patchi)
		{
			this->operator[](patchi).initEvaluate(Pstream::defaultCommsType);
		}

		// Block for any outstanding requests
		if
			(
				Pstream::parRun()
				&& Pstream::defaultCommsType == Pstream::commsTypes::nonBlocking
				)
		{
			Pstream::waitRequests(nReq);
		}

		forAll(*this, patchi)
		{
			this->operator[](patchi).evaluate(Pstream::defaultCommsType);
		}
	}
	else if (Pstream::defaultCommsType == Pstream::commsTypes::scheduled)
	{
		const lduSchedule& patchSchedule =
			bmesh_.mesh().globalData().patchSchedule();

		forAll(patchSchedule, patchEvali)
		{
			if (patchSchedule[patchEvali].init)
			{
				this->operator[](patchSchedule[patchEvali].patch)
					.initEvaluate(Pstream::commsTypes::scheduled);
			}
			else
			{
				this->operator[](patchSchedule[patchEvali].patch)
					.evaluate(Pstream::commsTypes::scheduled);
			}
		}
	}
	else
	{
		FatalErrorInFunction
			<< "Unsupported communications type "
			<< Pstream::commsTypeNames[Pstream::defaultCommsType]
			<< exit(FatalError);
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::wordList
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
types() const
{
	const FieldField<PatchField, Type>& pff = *this;

	wordList Types(pff.size());

	forAll(pff, patchi)
	{
		Types[patchi] = pff[patchi].type();
	}

	return Types;
}


template<class Type, template<class> class PatchField, class GeoMesh>
typename tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
boundaryInternalField() const
{
	typename GeometricField<Type, PatchField, GeoMesh>::Boundary
		BoundaryInternalField(*this);

	forAll(BoundaryInternalField, patchi)
	{
		BoundaryInternalField[patchi] ==
			this->operator[](patchi).patchInternalField();
	}

	return BoundaryInternalField;
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::LduInterfaceFieldPtrsList<Type>
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
interfaces() const
{
	LduInterfaceFieldPtrsList<Type> interfaces(this->size());

	forAll(interfaces, patchi)
	{
		if (isA<LduInterfaceField<Type>>(this->operator[](patchi)))
		{
			interfaces.set
			(
				patchi,
				&refCast<const LduInterfaceField<Type>>
				(
					this->operator[](patchi)
					)
			);
		}
	}

	return interfaces;
}


template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::lduInterfaceFieldPtrsList
tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
scalarInterfaces() const
{
	lduInterfaceFieldPtrsList interfaces(this->size());

	forAll(interfaces, patchi)
	{
		if (isA<lduInterfaceField>(this->operator[](patchi)))
		{
			interfaces.set
			(
				patchi,
				&refCast<const lduInterfaceField>
				(
					this->operator[](patchi)
					)
			);
		}
	}

	return interfaces;
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
writeEntry(const word& keyword, Ostream& os) const
{
	os << keyword << nl << token::BEGIN_BLOCK << incrIndent << nl;

	forAll(*this, patchi)
	{
		os << indent << this->operator[](patchi).patch().name() << nl
			<< indent << token::BEGIN_BLOCK << nl
			<< incrIndent << this->operator[](patchi) << decrIndent
			<< indent << token::END_BLOCK << endl;
	}

	os << decrIndent << token::END_BLOCK << endl;

	// Check state of IOstream
	os.check
	(
		"GeometricField<Type, PatchField, GeoMesh>::Boundary::"
		"writeEntry(const word& keyword, Ostream& os) const"
	);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
operator=
(
	const typename GeometricField<Type, PatchField, GeoMesh>::
	Boundary& bf
	)
{
	FieldField<PatchField, Type>::operator=(bf);
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
operator=
(
	typename GeometricField<Type, PatchField, GeoMesh>::
	Boundary&& bf
	)
{
	FieldField<PatchField, Type>::operator=(move(bf));
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
operator=
(
	const FieldField<PatchField, Type>& ptff
	)
{
	FieldField<PatchField, Type>::operator=(ptff);
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
operator=
(
	const Type& t
	)
{
	FieldField<PatchField, Type>::operator=(t);
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
operator==
(
	const typename GeometricField<Type, PatchField, GeoMesh>::
	Boundary& bf
	)
{
	forAll((*this), patchi)
	{
		this->operator[](patchi) == bf[patchi];
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
operator==
(
	const FieldField<PatchField, Type>& ptff
	)
{
	forAll((*this), patchi)
	{
		this->operator[](patchi) == ptff[patchi];
	}
}


template<class Type, template<class> class PatchField, class GeoMesh>
void tnbLib::GeometricField<Type, PatchField, GeoMesh>::Boundary::
operator==
(
	const Type& t
	)
{
	forAll((*this), patchi)
	{
		this->operator[](patchi) == t;
	}
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template<class Type, template<class> class PatchField, class GeoMesh>
tnbLib::Ostream& tnbLib::operator<<
(
	Ostream& os,
	const typename GeometricField<Type, PatchField, GeoMesh>::
	Boundary& bf
	)
{
	os << static_cast<const FieldField<PatchField, Type>&>(bf);
	return os;
}


// ************************************************************************* //