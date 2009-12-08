/**
 * @file MergeProcess.tcc
 * @brief
 * @author
 * @date    27/11/09 13:42
 *
 */

#include "MergeFunctors.hpp"
#include <tuttle/common/math/rectOp.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

using namespace boost::gil;

template<class View, class Functor>
MergeProcess<View, Functor>::MergeProcess( MergePlugin & instance )
	: tuttle::plugin::ImageGilProcessor<View>( instance ),
	tuttle::plugin::Progress( instance ),
	_plugin( instance )
{}

template<class View, class Functor>
void MergeProcess<View, Functor>::setupAndProcess( const OFX::RenderArguments& args )
{
	try
	{
		// Create views
		boost::scoped_ptr<OFX::Image> srcA( _plugin.getSrcClipA()->fetchImage( args.time ) );
		if( !srcA.get() )
			throw( ImageNotReadyException() );
		OfxRectI sBoundsA                      = srcA->getBounds();
		OFX::BitDepthEnum srcBitDepthA         = srcA->getPixelDepth();
		OFX::PixelComponentEnum srcComponentsA = srcA->getPixelComponents();

		boost::scoped_ptr<OFX::Image> srcB( _plugin.getSrcClipB()->fetchImage( args.time ) );
		if( !srcB.get() )
			throw( ImageNotReadyException() );
		OfxRectI sBoundsB                      = srcB->getBounds();
		OFX::BitDepthEnum srcBitDepthB         = srcB->getPixelDepth();
		OFX::PixelComponentEnum srcComponentsB = srcB->getPixelComponents();

		boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip()->fetchImage( args.time ) );
		if( !dst.get() )
			throw( ImageNotReadyException() );
		OfxRectI dBounds                      = dst->getBounds();
		OFX::BitDepthEnum dstBitDepth         = dst->getPixelDepth();
		OFX::PixelComponentEnum dstComponents = dst->getPixelComponents();

		// Make sure bit depths are the same
		if( srcBitDepthA != dstBitDepth || srcBitDepthB != srcBitDepthA ||
			srcComponentsA != dstComponents || srcComponentsB != srcComponentsA )
		{
			throw( BitDepthMismatchException() );
		}

		// Build sources view
		this->_srcViewA = interleaved_view( std::abs( sBoundsA.x2 - sBoundsA.x1 ), std::abs( sBoundsA.y2 - sBoundsA.y1 ),
		                                    static_cast < value_t* >( srcA->getPixelData() ),
		                                    srcA->getRowBytes() );

		this->_srcViewB = interleaved_view( std::abs( sBoundsB.x2 - sBoundsB.x1 ), std::abs( sBoundsB.y2 - sBoundsB.y1 ),
		                                    static_cast < value_t* >( srcB->getPixelData() ),
		                                    srcB->getRowBytes() );

		// Build destination view
		this->_dstView = interleaved_view( std::abs( dBounds.x2 - dBounds.x1 ), std::abs( dBounds.y2 - dBounds.y1 ),
		                                   static_cast < value_t* >( dst->getPixelData() ),
		                                   dst->getRowBytes() );

		// Set the render window
		this->setRenderWindow( args.renderWindow );
		// Call the base class process member
		this->process();
	}
	catch( PluginException e )
	{
		COUT_EXCEPTION( e );
	}
}

/**
 * @brief Function called by rendering thread each time
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View, class Functor>
void MergeProcess<View, Functor>::multiThreadProcessImages( OfxRectI procWindow )
{
	try
	{
		OfxRectI rA = { procWindow.x1, procWindow.y1, this->_srcViewA.width(), this->_srcViewA.height() };
		OfxRectI rB = { procWindow.x1, procWindow.y1, this->_srcViewB.width(), this->_srcViewB.height() };
		OfxRectI rC = { procWindow.x1, procWindow.y1, this->_dstView.width(), this->_dstView.height() };
		OfxRectI rInterAB = intersection( rA, rB );
		OfxRectI rIntersect = intersection( rC, rInterAB );
		View srcA = subimage_view( this->_srcViewA,
								   procWindow.x1 - tuttle::plugin::ImageGilProcessor<View>::_renderWindow.x1,
								   procWindow.y1 - tuttle::plugin::ImageGilProcessor<View>::_renderWindow.y1,
		                           procWindow.x2 - procWindow.x1,
		                           procWindow.y2 - procWindow.y1 );
		View srcB = subimage_view( this->_srcViewB,
								   procWindow.x1 - tuttle::plugin::ImageGilProcessor<View>::_renderWindow.x1,
								   procWindow.y1 - tuttle::plugin::ImageGilProcessor<View>::_renderWindow.y1,
		                           procWindow.x2 - procWindow.x1,
		                           procWindow.y2 - procWindow.y1 );
		View dst = subimage_view( this->_dstView,
								  procWindow.x1 - tuttle::plugin::ImageGilProcessor<View>::_renderWindow.x1,
								  procWindow.y1 - tuttle::plugin::ImageGilProcessor<View>::_renderWindow.y1,
								  procWindow.x2 - procWindow.x1,
								  procWindow.y2 - procWindow.y1 );
//		Functor fun = Functor();
//		merge_pixels( srcA, srcB, dst, Functor() );
	}
	catch( PluginException err )
	{
		COUT_EXCEPTION( err );
	}
}

}
}
}