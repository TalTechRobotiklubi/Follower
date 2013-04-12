#include "SpineData.h"

#include <string>
#include <algorithm>

SpineData::SpineData(void) : Data(Data::SpineDataT),
	ballSensed_(false),	startButton_(false)
{
}

void SpineData::GetOdometryClicks(int* clicks) const
{
	clicks[0] = odometryClicks_[1];
	clicks[1] = odometryClicks_[0];
	clicks[2] = odometryClicks_[2];
}

void SpineData::GetOdometryMove( double* out_move ) const
{
	memcpy(out_move, odometryMove_, 3*sizeof(double));
}

void SpineData::SetOdometryMove( double* move )
{
	memcpy(odometryMove_, move, 3*sizeof(double));
}
