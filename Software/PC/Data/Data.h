#pragma once

/* logimine
#include <boost/serialization/serialization.hpp>
#include <boost/archive/basic_archive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
*/

class Data
{
public:
	enum DataType { VisionDataT, SpineDataT, VisionFrameDataT, UndefinedT };

	Data(DataType type) : type_(type) {};
	Data() : type_(UndefinedT) {};

	virtual ~Data() {};

	DataType	type() const			{ return type_; }

	void		SetTime(double time)	{ time_ = time; }
	double		time() const			{ return time_; }
private:
	DataType	type_;
	double		time_;

	// Necessary for boost::serialization
	virtual void foo() {};

	/*friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & type_;
		ar & time_;
	}*/
};