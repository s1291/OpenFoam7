#pragma once
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

\*---------------------------------------------------------------------------*/

template<class ParcelType>
template<class TrackCloudType>
inline tnbLib::ReactingParcel<ParcelType>::trackingData::trackingData
(
	const TrackCloudType& cloud,
	trackPart part
)
	:
	ParcelType::trackingData(cloud, part),
	pInterp_
	(
		interpolation<scalar>::New
		(
			cloud.solution().interpolationSchemes(),
			cloud.p()
		)
	),
	pc_(Zero)
{}


template<class ParcelType>
inline const tnbLib::interpolation<tnbLib::scalar>&
tnbLib::ReactingParcel<ParcelType>::trackingData::pInterp() const
{
	return pInterp_();
}


template<class ParcelType>
inline tnbLib::scalar tnbLib::ReactingParcel<ParcelType>::trackingData::pc() const
{
	return pc_;
}


template<class ParcelType>
inline tnbLib::scalar& tnbLib::ReactingParcel<ParcelType>::trackingData::pc()
{
	return pc_;
}


// ************************************************************************* //
