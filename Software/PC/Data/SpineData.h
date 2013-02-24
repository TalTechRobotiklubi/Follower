#pragma once
#include "Data.h"

class SpineData : public Data
{
public:
	SpineData(void);

	bool					GetBallSensed()		const { return ballSensed_; } 
	bool					GetStartButton()	const { return startButton_; }

	void					GetOdometryClicks(int* clicks) const;
	void					GetOdometryMove(double* out_move) const;
	//Eigen::Vector3d			GetOdometryMove() const;
	void					SetOdometryMove(double* move);

	void					SetBallSensed(bool ballSensed)		{ ballSensed_ = ballSensed; }
	void					SetStartButton(bool startButton)	{ startButton_ = startButton; }

protected:
	bool					ballSensed_;
	bool					startButton_;

	int						odometryClicks_[3];
	double					odometryMove_[3];

    /*
    logimine
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<Data>(*this);
		ar & startButton_;
		ar & ballSensed_;
		ar & odometryMove_;
	}
    */
};
