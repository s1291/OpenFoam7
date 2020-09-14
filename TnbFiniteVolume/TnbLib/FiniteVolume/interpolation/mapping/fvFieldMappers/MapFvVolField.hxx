#pragma once
#ifndef _MapFvVolField_Header
#define _MapFvVolField_Header

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

Description
	Map volume internal field on topology change.  This is a partial
	template specialisation, see MapGeometricFields.

\*---------------------------------------------------------------------------*/

#include <Field.hxx>
#include <volMesh.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	template<class Type, class MeshMapper>
	class MapInternalField<Type, MeshMapper, volMesh>
	{
	public:

		MapInternalField()
		{}

		void operator()
			(
				Field<Type>& field,
				const MeshMapper& mapper
				) const;
	};


	template<class Type, class MeshMapper>
	void MapInternalField<Type, MeshMapper, volMesh>::operator()
		(
			Field<Type>& field,
			const MeshMapper& mapper
			) const
	{
		if (field.size() != mapper.volMap().sizeBeforeMapping())
		{
			FatalErrorInFunction
				<< "Incompatible size before mapping.  Field size: " << field.size()
				<< " map size: " << mapper.volMap().sizeBeforeMapping()
				<< abort(FatalError);
		}

		mapper.volMap()(field, field);
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_MapFvVolField_Header
