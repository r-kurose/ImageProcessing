// -*- C++ -*-
/*!
 * @file  Chromakey.cpp
 * @brief Chromakey image component
 * @date $Date$
 *
 * $Id$
 */

#include "Chromakey.h"

// Module specification
// <rtc-template block="module_spec">
static const char* chromakey_spec[] =
  {
    "implementation_id", "Chromakey",
    "type_name",         "Chromakey",
    "description",       "Chromakey image component",
    "version",           "1.1.0",
    "vendor",            "AIST",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.lower_blue", "0",
    "conf.default.upper_blue", "0",
    "conf.default.lower_green", "0",
    "conf.default.upper_green", "0",
    "conf.default.lower_red", "0",
    "conf.default.upper_red", "0",
    // Widget
    "conf.__widget__.lower_blue", "slider.1",
    "conf.__widget__.upper_blue", "slider.1",
    "conf.__widget__.lower_green", "slider.1",
    "conf.__widget__.upper_green", "slider.1",
    "conf.__widget__.lower_red", "slider.1",
    "conf.__widget__.upper_red", "slider.1",
    // Constraints
    "conf.__constraints__.lower_blue", "0<=x<=255",
    "conf.__constraints__.upper_blue", "0<=x<=255",
    "conf.__constraints__.lower_green", "0<=x<=255",
    "conf.__constraints__.upper_green", "0<=x<=255",
    "conf.__constraints__.lower_red", "0<=x<=255",
    "conf.__constraints__.upper_red", "0<=x<=255",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Chromakey::Chromakey(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_originalIn("original_image", m_image_original),
    m_image_backIn("background_image", m_image_back),
    m_image_outputOut("chromakey_image", m_image_output)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Chromakey::~Chromakey()
{
}



RTC::ReturnCode_t Chromakey::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("original_image", m_image_originalIn);
  addInPort("background_image", m_image_backIn);
  
  // Set OutPort buffer
  addOutPort("chromakey_image", m_image_outputOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("lower_blue", m_nLowerBlue, "0");
  bindParameter("upper_blue", m_nUpperBlue, "0");
  bindParameter("lower_green", m_nLowerGreen, "0");
  bindParameter("upper_green", m_nUpperGreen, "0");
  bindParameter("lower_red", m_nLowerRed, "0");
  bindParameter("upper_red", m_nUpperRed, "0");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Chromakey::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Chromakey::onActivated(RTC::UniqueId ec_id)
{

  m_in_height         = 0;
  m_in_width          = 0;
  m_in2_height        = 0;
  m_in2_width         = 0;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Chromakey::onDeactivated(RTC::UniqueId ec_id)
{
  /* イメージ用メモリの解放 */
	if (!m_image_buff.empty())
	{
		m_image_buff.release();
	}
	if (!m_image_extracted.empty())
	{
		m_image_extracted.release();
	}
	if (!m_image_mask.empty())
	{
		m_image_mask.release();
	}
	if (!m_image_inverseMask.empty())
	{
		m_image_inverseMask.release();
	}
	if (!m_image_BG_in.empty())
	{
		m_image_BG_in.release();
	}
	if (!m_image_BG.empty())
	{
		m_image_BG.release();
	}
	if (!m_image_extractedBG.empty())
	{
		m_image_extractedBG.release();
	}
	if (!m_image_destination.empty())
	{
		m_image_destination.release();
	}


  return RTC::RTC_OK;
}


RTC::ReturnCode_t Chromakey::onExecute(RTC::UniqueId ec_id)
{
  // Common CV actions
  // Port for Background image
  if (m_image_backIn.isNew()) 
  {
    /* InPortデータの読み込み */
    m_image_backIn.read();

    /* サイズが変わったときだけ再生成する */
    if(m_in2_height != m_image_back.height || m_in2_width != m_image_back.width)
    {
      printf("[onExecute] Size of background image is not match!\n");

      m_in2_height = m_image_back.height;
      m_in2_width  = m_image_back.width;



      /* サイズ変換のためTempメモリーを用意する */
	  m_image_BG_in.create(cv::Size(m_in2_width, m_in2_height), CV_8UC3);

    }

    /* InPortの画像データをIplImageのimageDataにコピー */
    memcpy(m_image_BG_in.data,(void *)&(m_image_back.pixels[0]), m_image_back.pixels.length());
  }

  /* 新しいデータのチェック */
  if (m_image_originalIn.isNew()) 
  {
    m_image_originalIn.read();

    /* サイズが変わったときだけ再生成する */
    if(m_in_height != m_image_original.height || m_in_width != m_image_original.width)
    {
      printf("[onExecute] Size of input image is not match!\n");

      m_in_height = m_image_original.height;
      m_in_width  = m_image_original.width;





	  m_image_buff.create(cv::Size(m_in_width, m_in_height), CV_8UC3);
	  m_image_extracted.create(cv::Size(m_in_width, m_in_height), CV_8UC3);
	  m_image_mask.create(cv::Size(m_in_width, m_in_height), CV_8UC1);
	  m_image_inverseMask.create(cv::Size(m_in_width, m_in_height), CV_8UC1);
	  m_image_BG.create(cv::Size(m_in_width, m_in_height), CV_8UC3);
	  m_image_extractedBG.create(cv::Size(m_in_width, m_in_height), CV_8UC3);
	  m_image_destination.create(cv::Size(m_in_width, m_in_height), CV_8UC3);
    }

    // Resize background image to fit Camera image
    if(!m_image_BG_in.empty())
		cv::resize(m_image_BG_in, m_image_BG, m_image_BG.size());

    memcpy(m_image_buff.data,(void *)&(m_image_original.pixels[0]),m_image_original.pixels.length());

    // Anternative actions
	cv::Scalar lowerValue = cv::Scalar(m_nLowerBlue, m_nLowerGreen, m_nLowerRed);
	cv::Scalar upperValue = cv::Scalar(m_nUpperBlue + 1, m_nUpperGreen + 1, m_nUpperRed + 1);

    /* RGB各チャンネルごとに範囲内の値以外の画素をマスクに設定する */
	cv::inRange(m_image_buff, lowerValue, upperValue, m_image_mask);

    /* 背景画像のうち合成する物体部分の画素値を0にする */

	m_image_extractedBG = cv::Mat::zeros(m_image_extractedBG.size(), CV_8UC3);
	m_image_extractedBG.copyTo(m_image_BG, m_image_mask);
    

    /* マスク画像の0と1を反転する */
	cv::bitwise_not(m_image_mask, m_image_inverseMask);
    

    /* トラックバーの条件を満たす合成物体が抽出された画像を作成 */
	m_image_extracted = cv::Mat::zeros(m_image_extractedBG.size(), CV_8UC3);
	m_image_extracted.copyTo(m_image_buff, m_image_inverseMask);
    

    /* 背景画像と合成物体画像の合成 */
    cv::add( m_image_extractedBG, m_image_extracted, m_image_destination);
       
    // Prepare to out data
	int len = m_image_destination.channels() * m_image_destination.size().width * m_image_destination.size().height;
            
    m_image_output.pixels.length(len);        
	m_image_output.width = m_image_destination.size().width;
	m_image_output.height = m_image_destination.size().height;
    memcpy((void *)&(m_image_output.pixels[0]), m_image_destination.data,len);

    m_image_outputOut.write();
  }

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Chromakey::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void ChromakeyInit(RTC::Manager* manager)
  {
    coil::Properties profile(chromakey_spec);
    manager->registerFactory(profile,
                             RTC::Create<Chromakey>,
                             RTC::Delete<Chromakey>);
  }
  
};


