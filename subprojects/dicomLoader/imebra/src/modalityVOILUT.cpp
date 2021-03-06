/*

Imebra 2011 build 2013-09-04_11-02-26

Imebra: a C++ Dicom library

Copyright (c) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 by Paolo Brandoli/Binarno s.p.
All rights reserved.

This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

-------------------

If you want to use Imebra commercially then you have to buy the commercial
 support available at http://imebra.com

After you buy the commercial support then you can use Imebra according
 to the terms described in the Imebra Commercial License Version 1.
A copy of the Imebra Commercial License Version 1 is available in the
 documentation pages.

Imebra is available at http://imebra.com

The author can be contacted by email at info@binarno.com or by mail at
 the following address:
 Paolo Brandoli
 Rakuseva 14
 1000 Ljubljana
 Slovenia



*/

/*! \file modalityVOILUT.cpp
    \brief Implementation of the class modalityVOILUT.

*/

#include "../../base/include/exception.h"
#include "../include/modalityVOILUT.h"
#include "../include/dataSet.h"
#include "../include/colorTransformsFactory.h"

namespace puntoexe
{

namespace imebra
{

namespace transforms
{


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Modality VOILUT transform
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
modalityVOILUT::modalityVOILUT(ptr<dataSet> pDataSet):
		m_pDataSet(pDataSet), m_voiLut(pDataSet->getLut(0x0028, 0x3000, 0)), m_rescaleIntercept(pDataSet->getDouble(0x0028, 0, 0x1052, 0x0)), m_rescaleSlope(1.0), m_bEmpty(true)

{
	// Only monochrome images can have the modality voi-lut
	///////////////////////////////////////////////////////
	std::wstring colorSpace(pDataSet->getUnicodeString(0x0028, 0x0, 0x0004, 0x0));
	if(!colorTransforms::colorTransformsFactory::isMonochrome(colorSpace))
	{
		return;
	}

	ptr<handlers::dataHandler> rescaleHandler(m_pDataSet->getDataHandler(0x0028, 0, 0x1053, 0x0, false));
	if(rescaleHandler != 0)
	{
		m_rescaleSlope = rescaleHandler->getDouble(0);
		m_bEmpty = false;
	}
	if(m_voiLut != 0 && m_voiLut->getSize() != 0)
	{
		m_bEmpty = false;
	}

}

bool modalityVOILUT::isEmpty()
{
	return m_bEmpty;
}


ptr<image> modalityVOILUT::allocateOutputImage(ptr<image> pInputImage, imbxUint32 width, imbxUint32 height)
{
	if(isEmpty())
	{
		ptr<image> newImage(new image);
		newImage->create(width, height, pInputImage->getDepth(), pInputImage->getColorSpace(), pInputImage->getHighBit());
		return newImage;
	}

	// LUT
	///////////////////////////////////////////////////////////
	if(m_voiLut != 0 && m_voiLut->getSize() != 0 && m_voiLut->checkValidDataRange())
	{
		imbxUint8 bits(m_voiLut->getBits());

		// Look for negative outputs
		bool bNegative(false);
		for(imbxInt32 index(m_voiLut->getFirstMapped()), size(m_voiLut->getSize()); !bNegative && size != 0; --size, ++index)
		{
			bNegative = (m_voiLut->mappedValue(index) < 0);
		}

		image::bitDepth depth;
		if(bNegative)
		{
			depth = bits > 8 ? image::depthS16 : image::depthS8;
		}
		else
		{
			depth = bits > 8 ? image::depthU16 : image::depthU8;
		}
		ptr<image> returnImage(new image);
		returnImage->create(width, height, depth, L"MONOCHROME2", bits - 1);
		return returnImage;
	}

	// Rescale
	///////////////////////////////////////////////////////////
	if(m_rescaleSlope == 0)
	{
		ptr<image> returnImage(new image);
		returnImage->create(width, height, pInputImage->getDepth(), L"MONOCHROME2", pInputImage->getHighBit());
		return returnImage;
	}

	image::bitDepth inputDepth(pInputImage->getDepth());
	imbxUint32 highBit(pInputImage->getHighBit());

	imbxInt32 value0 = 0;
	imbxInt32 value1 = ((imbxInt32)1 << (highBit + 1)) - 1;
	if(inputDepth == image::depthS16 || inputDepth == image::depthS8)
	{
		value0 = ((imbxInt32)(-1) << highBit);
		value1 = ((imbxInt32)1 << highBit);
	}
	imbxInt32 finalValue0((imbxInt32) ((double)value0 * m_rescaleSlope + m_rescaleIntercept + 0.5) );
	imbxInt32 finalValue1((imbxInt32) ((double)value1 * m_rescaleSlope + m_rescaleIntercept + 0.5) );

	imbxInt32 minValue, maxValue;
	if(finalValue0 < finalValue1)
	{
		minValue = finalValue0;
		maxValue = finalValue1;
	}
	else
	{
		minValue = finalValue1;
		maxValue = finalValue0;
	}

	ptr<image> returnImage(new image);
	if(minValue >= 0 && maxValue <= 255)
	{
		returnImage->create(width, height, image::depthU8, L"MONOCHROME2", 7);
		return returnImage;
	}
	if(minValue >= -128 && maxValue <= 127)
	{
		returnImage->create(width, height, image::depthS8, L"MONOCHROME2", 7);
		return returnImage;
	}
	if(minValue >= 0 && maxValue <= 65535)
	{
		returnImage->create(width, height, image::depthU16, L"MONOCHROME2", 15);
		return returnImage;
	}
	if(minValue >= -32768 && maxValue <= 32767)
	{
		returnImage->create(width, height, image::depthS16, L"MONOCHROME2", 15);
		return returnImage;
	}
	returnImage->create(width, height, image::depthS32, L"MONOCHROME2", 31);
	return returnImage;
}



} // namespace transforms

} // namespace imebra

} // namespace puntoexe
