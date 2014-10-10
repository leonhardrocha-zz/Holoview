#include "vld.h"
#ifndef _FTIMAGEADAPTER_H
#define _FTIMAGEADAPTER_H
#pragma once
#include "OvImageAdapter.h"
#include <FaceTrackLib.h>

class FTImageAdapter : OvImageAdapter
{
public:
    FTImageAdapter() : OvImageAdapter() {}; /**< Default constructor with no parameters */
    FTImageAdapter(IFTImage* image) : OvImageAdapter() { SetImage(image);}
    virtual ~FTImageAdapter() { if (!m_image->IsAttached()) { m_image->Release(); }}/**< Destructor */
    void SetImage(IFTImage* image) 
    {
        m_image = image; 
        mChannels = (int)(m_image->GetBytesPerPixel());
        mWidth = (int)(m_image->GetWidth());
        mHeight = (int)(m_image->GetHeight());
        mDataType = m_image->GetFormat() == FTIMAGEFORMAT_UINT8_R8G8B8 ? OV_DATA_UINT8 : OV_DATA_UNKNOWN;
    }

    IFTImage* GetImage() 
    {
        return m_image; 
    }


  /** 
  * Enumeration of allowed data types.
  * @see mDataType
  */
  //enum OvDataType {
  //  OV_DATA_UNKNOWN, /**< unknown */
  //  OV_DATA_UINT8,   /**< unsigned char */
  //  OV_DATA_INT8,	 /**< char */
  //  OV_DATA_UINT16,  /**< short */
  //  OV_DATA_INT16,   /**< unsigned short */
  //  OV_DATA_UINT32,  /**< int */
  //  OV_DATA_INT32,   /**< unsigned int */
  //  OV_DATA_UINT64,  /**< unsigned long long */
  //  OV_DATA_INT64,   /**< long long */
  //  OV_DATA_FLOAT32, /**< float */
  //  OV_DATA_DOUBLE64 /**< double */
  //};

  /**
  * Returns height, width and number of color channels of image.
  * @param height height of image
  * @param width width of image
  * @param nColorChannels number of color channels of image
  */
  virtual void getSize(int & height, int & width, int & nColorChannels) const; 

  /**
  * Returns data type of image.
  * @param dataType data type of image (from enum OvDataType)
  * @see OvDataType
  */
  virtual void getDataType(OvImageAdapter::OvDataType & dataType) const;

  /**
  * Returns a pixel value at a particular row, column and color channel.
  * This is a pure virtual function.
  * @param row row of the image
  * @param column column of the image
  * @param channel channel of the image
  * @return pixel value (type double)
  */
  virtual double getPixel(int row, int column, int channel) const 
  {
      BYTE* buffer = m_image.GetBuffer();
      UINT32 index = channel * m_image.GetStride() * row + column;
      UINT32 pixel = buffer[index];
      return (double)pixel;
  };

  /**
  * Sets a pixel value at a particular row, column and color channel.
  * This is a pure virtual function.
  * @param value value to be set
  * @param row row of the image
  * @param column column of the image
  * @param channel channel of the image	
  */
  virtual void   setPixel(double value, int row, int column, int channel)
  {
      BYTE* buffer = m_image.GetBuffer();
      UINT32 index = channel * m_image.GetStride() * row + column;
      buffer[index]  = UINT8(value);
  };

protected:
  int  mHeight;			/**< height of the image */
  int  mWidth;			/**< width of the image */
  int  mChannels;			/**< number of color channels (e.g., 1 for grayscale, 3 for RGB) */

  /**
  * Data format of a pixel channel
  * @see OvDataType
  */
  OvDataType mDataType;
  IFTImage* m_image;
};
#endif