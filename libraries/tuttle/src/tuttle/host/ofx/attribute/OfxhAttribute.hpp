#ifndef _TUTTLE_HOST_OFX_ATTRIBUTE_HPP_
#define _TUTTLE_HOST_OFX_ATTRIBUTE_HPP_

#include "OfxhAttributeDescriptor.hpp"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/type_traits/is_virtual_base_of.hpp>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

enum EChange
{
	eChangeNone,
	eChangeTime,
	eChangeUserEdited,
	eChangePluginEdited,
};

inline std::string mapEChangeToString( const EChange change )
{
	switch( change )
	{
		case eChangeNone:
			return "eChangeNone";
		case eChangeTime:
			return kOfxChangeTime;
		case eChangeUserEdited:
			return kOfxChangeUserEdited;
		case eChangePluginEdited:
			return kOfxChangePluginEdited;
	}
}

inline EChange mapStringToEChange( const std::string& change )
{
	if( change == kOfxChangeTime )
		return eChangeTime;
	else if( change == kOfxChangeUserEdited )
		return eChangeUserEdited;
	else if( change == kOfxChangePluginEdited )
		return eChangePluginEdited;
	else
		return eChangeNone;
}


class OfxhAttribute : virtual public OfxhAttributeAccessor
{
public:
	typedef OfxhAttribute This;
protected:
	OfxhAttribute(){}
public:
	OfxhAttribute( const property::OfxhSet& properties );
	OfxhAttribute( const OfxhAttributeDescriptor& desc );
	virtual ~OfxhAttribute() = 0;

	virtual bool operator==( const This& other ) const
	{
		if( _properties != other._properties )
			return false;
		return true;
	}
	bool operator!=( const This& other ) const { return !This::operator==( other ); }

protected:
	property::OfxhSet _properties;

protected:
	void setProperties( const property::OfxhSet& properties )
	{
		_properties = properties;
		assert( getAttributeType().c_str() );
	}

public:
	const property::OfxhSet& getProperties() const
	{
		return _properties;
	}

	property::OfxhSet& getEditableProperties()
	{
		return _properties;
	}

	virtual void copyValues( const This& other )
	{
		_properties.copyValues(other._properties);
	}
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive &ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_NVP(_properties);
	}
};

}
}
}
}

#endif