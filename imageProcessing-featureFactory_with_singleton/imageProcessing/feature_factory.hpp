#ifndef _FEATURE_FACTORY_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "opencv2/core/types_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/calib3d/calib3d_c.h"

#include <iostream>
#include <map>

#include <afxmt.h>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

using std::map;
using std::string;
using std::vector;

// simplest implementing of lazy singleton
class CLazySingleton
{
public:
	template<typename T>
	static T& getInstance ( )
	{
		static T	_instance;	// static instance

		return	_instance;
	}
};

// class of lazy singleton, with double-checked locking
template<typename T>
class CLazySingletong_dcl
{
private:
	static T*	_instance;
	CMutex		mtx;

public:
	// double-checked locking
	static T*	getInstance ( )
	{
		if ( NULL == _instance )
		{
			mtx.lock ( );

			if ( NULL == _instance )
			{
				_instance	= new T;
			}

			mtx.unlock ( );
		}

		return	_instance;
	}

	static void releaseInstance ( )
	{
		mtx.lock ( );

		if ( NULL != _instance )
		{
			delete	_instance;

			_instance	= NULL;
		}

		mtx.unlock ( );
	}

};

// factory for feature detectors, extractors and matchers
template <typename T>
class featureRegistry
{
public:
	typedef	Ptr<T> ( *Creator )();
	typedef std::map<string, Creator>	creatorRegistry;

	static creatorRegistry& detectorRegistry ( )
	{
		static creatorRegistry*	g_registry_	= new creatorRegistry;
		return	*g_registry_;
	}

	static creatorRegistry& extractorRegistry ( )
	{
		static creatorRegistry*	g_registry_	= new creatorRegistry;
		return	*g_registry_;
	}

	static creatorRegistry& matcherRegistry ( )
	{
		static creatorRegistry*	g_registry_	= new creatorRegistry;
		return	*g_registry_;
	}

	// adds a detector
	static void addDetector ( const string& type, Creator creator )
	{
		creatorRegistry&	registry	= detectorRegistry ( );

		registry [ type ]	= creator;
	}

	// adds an extractor
	static void addExtractor ( const string& type, Creator creator )
	{
		creatorRegistry&	registry	= extractorRegistry ( );

		registry [ type ]	= creator;
	}

	// adds a matcher
	static void addMatcher ( const string& type, Creator creator )
	{
		creatorRegistry&	registry	= matcherRegistry ( );

		registry [ type ]	= creator;
	}

	// get a feature detector via type
	static Ptr<T>	createDetector ( const string& type )
	{
		creatorRegistry&	registry	= detectorRegistry ( );
		return	registry [ type ] ( );
	}

	// get a feature extractor
	static Ptr<T>	createExtractor ( const string& type )
	{
		creatorRegistry&	registry	= extractorRegistry ( );
		return	registry [ type ] ( );
	}

	// get a feature extractor
	static Ptr<T>	createMatcher ( const string& type )
	{
		creatorRegistry&	registry	= matcherRegistry ( );
		return	registry [ type ] ( );
	}

	// detector type list
	static vector<string>	detectorTypeList ( )
	{
		creatorRegistry&	registry	= detectorRegistry ( );
		vector<string>		detector_types;
		for ( typename creatorRegistry::iterator iter = registry.begin ( );
			  iter != registry.end ( ); ++iter )
		{
			detector_types.push_back ( iter->first );
		}
		return	detector_types;
	}

	// extractor type list
	static vector<string>	extractorTypeList ( )
	{
		creatorRegistry&	registry	= extractorRegistry ( );
		vector<string>		extractor_types;
		for ( typename creatorRegistry::iterator iter = registry.begin ( );
			  iter != registry.end ( ); ++iter )
		{
			extractor_types.push_back ( iter->first );
		}
		return	extractor_types;
	}

	// matcher type list
	static vector<string>	matcherTypeList ( )
	{
		creatorRegistry&	registry	= matcherRegistry ( );
		vector<string>		matcher_types;
		for ( typename creatorRegistry::iterator iter = registry.begin ( );
			  iter != registry.end ( ); ++iter )
		{
			matcher_types.push_back ( iter->first );
		}
		return	matcher_types;
	}

private:
	// feature registry should never be instantiated
	// everything is done with its static variables
	featureRegistry ( ) { }
};

template <typename T>
class detectorRegisterer
{
public:
	detectorRegisterer ( const string& type, Ptr<T> ( *creator )() )
	{
		featureRegistry<T>::addDetector ( type, creator );
	}
};

template <typename T>
class extractorRegisterer
{
public:
	extractorRegisterer ( const string& type, Ptr<T> ( *creator )() )
	{
		featureRegistry<T>::addExtractor ( type, creator );
	}
};

template <typename T>
class matcherRegisterer
{
public:
	matcherRegisterer ( const string& type, Ptr<T> ( *creator )() )
	{
		featureRegistry<T>::addMatcher ( type, creator );
	}
};


// register macro: register a detector class by its creator
#define REGISTER_DETECTOR_CREATOR(type, creator, T)							\
	static detectorRegisterer<T> g_detector_creator_##type(#type, creator);

// register a detector class by its classname: make a creator
#define REGISTER_DETECTOR_CLASS(type, T)		\
	template <typename T>						\
	Ptr<T> Creator_##type##Detector()			\
	{											\
		return Ptr<T>(new type##Detector());	\
	}											\
	REGISTER_DETECTOR_CREATOR(type, Creator_##type##Detector, T)


// register macro: register an extractor class by its creator
#define REGISTER_EXTRACTOR_CREATOR(type, creator, T)						\
	static extractorRegisterer<T> g_extractor_creator_##type(#type, creator);

// register an extractor class by its classname: make a creator
#define REGISTER_EXTRACTOR_CLASS(type, T)		\
	template <typename T>						\
	Ptr<T> Creator_##type##Extractor()			\
	{											\
		return Ptr<T>(new type##Extractor());	\
	}											\
	REGISTER_EXTRACTOR_CREATOR(type, Creator_##type##Extractor, T)


// register macro: register a matcher class by its creator
#define REGISTER_MATCHER_CREATOR(type, creator, T)							\
	static matcherRegisterer<T> g_matcher_creator_##type(#type, creator);

// register a matcher class by its classname: make a creator
#define REGISTER_MATCHER_CLASS(type, T)			\
	template <typename T>						\
	Ptr<T> Creator_##type##Matcher()			\
	{											\
		return Ptr<T>(new type##Matcher());		\
	}											\
	REGISTER_MATCHER_CREATOR(type, Creator_##type##Matcher, T)


#endif // !_FEATURE_FACTORY_H_

