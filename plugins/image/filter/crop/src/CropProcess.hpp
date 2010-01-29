/**
 * @file CropProcess.hpp
 * @brief
 * @author
 * @date    01/10/09 14:14
 *
 */
#ifndef VOLET_PROCESS_HPP
#define VOLET_PROCESS_HPP

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

/**
 * @brief Base class
 *
 */
template<class View>
class CropProcess : public tuttle::plugin::ImageGilProcessor<View>,
	public tuttle::plugin::Progress
{
typedef typename View::value_type value_t;

protected:
	CropPlugin&          _plugin;        ///< Rendering plugin
	View _srcView;      ///< Source view
	OfxRectI _srcBounds;    ///< Current source bounds
	OfxRectI _srcROD;       ///< Current source bounds
	OfxRectD _clipROD;      ///< Source clip region of definition
	OfxPointD _renderScale;  ///< Render scale
	double _par;          ///< Pixel aspect ratio
	OFX::IntParam*        _upBand;        ///< Up band size
	OFX::IntParam*        _downBand;      ///< Down band size
	OFX::IntParam*        _leftBand;      ///< Left band size
	OFX::IntParam*        _rightBand;     ///< Right band size
	OFX::BooleanParam*    _anamorphic;    ///< Anormorphic (stretch)

public:
	CropProcess<View>( CropPlugin & instance );

	// set up and run a processor
	void setupAndProcess( const OFX::RenderArguments& args );

	// Do some processing
	void multiThreadProcessImages( OfxRectI procWindow );

private:
	OfxRectD getCrop() const;
};

}
}
}

#include "CropProcess.tcc"

#endif  // VOLET_PROCESS_HPP
